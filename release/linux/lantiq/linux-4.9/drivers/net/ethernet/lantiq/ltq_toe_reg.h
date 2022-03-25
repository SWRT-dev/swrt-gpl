//-----------------------------------------------------------------------------
// LSD Generator
//-----------------------------------------------------------------------------
// Perl Package        : LSD::generator::targetC (v1.1)
// LSD Source          : D:/Users/shij/Perforce/l1033.grx500.win.v_shij.priv.dfv.grx500.dfv/ipg_lsd/lsd_sys/source/xml/reg_files/TSO.xml
// Register File Name  : TOE_CTRL
// Register File Title : TOE module Register Description
// Register Width      : 32
// Note                : Doxygen compliant comments
//-----------------------------------------------------------------------------

#ifndef _TOE_CTRL_H
#define _TOE_CTRL_H

//! \defgroup TOE_CTRL Register File TOE_CTRL - TOE module Register Description
//! @{


//! \defgroup PORT_REQ_CMD_REG0_0 Register PORT_REQ_CMD_REG0_0 - Port 0 request command register 0
//! @{

//! Register Offset (relative)
#define PORT_REQ_CMD_REG0_0 0x0

//! Register Reset Value
#define PORT_REQ_CMD_REG0_0_RST 0x00000000u

//! Field LEN - Data Block/Segment Length
#define PORT_REQ_CMD_REG0_0_LEN_POS 0
//! Field LEN - Data Block/Segment Length
#define PORT_REQ_CMD_REG0_0_LEN_MASK 0xFFFFFu

//! Field CHUNK_SIZ - Chunk size index
#define PORT_REQ_CMD_REG0_0_CHUNK_SIZ_POS 20
//! Field CHUNK_SIZ - Chunk size index
#define PORT_REQ_CMD_REG0_0_CHUNK_SIZ_MASK 0x700000u

//! Field SIOC - IOCU1 Address Translation for Source Address
#define PORT_REQ_CMD_REG0_0_SIOC_POS 23
//! Field SIOC - IOCU1 Address Translation for Source Address
#define PORT_REQ_CMD_REG0_0_SIOC_MASK 0x800000u
//! Constant NIL - no translation (default)
#define CONST_PORT_REQ_CMD_REG0_0_SIOC_NIL 0x0
//! Constant SIOC - Do translation
#define CONST_PORT_REQ_CMD_REG0_0_SIOC_SIOC 0x1

//! Field LAST - Last fragment of gathering
#define PORT_REQ_CMD_REG0_0_LAST_POS 24
//! Field LAST - Last fragment of gathering
#define PORT_REQ_CMD_REG0_0_LAST_MASK 0x1000000u
//! Constant NIL - not (default)
#define CONST_PORT_REQ_CMD_REG0_0_LAST_NIL 0x0
//! Constant LAST - Last fragment of gathering
#define CONST_PORT_REQ_CMD_REG0_0_LAST_LAST 0x1

//! Field G - Indication of gathering
#define PORT_REQ_CMD_REG0_0_G_POS 25
//! Field G - Indication of gathering
#define PORT_REQ_CMD_REG0_0_G_MASK 0x2000000u
//! Constant NIL - not (default)
#define CONST_PORT_REQ_CMD_REG0_0_G_NIL 0x0
//! Constant GATH - fragment of gathering
#define CONST_PORT_REQ_CMD_REG0_0_G_GATH 0x1

//! Field DIOC - Destination address needs to be translated into IOCU range
#define PORT_REQ_CMD_REG0_0_DIOC_POS 26
//! Field DIOC - Destination address needs to be translated into IOCU range
#define PORT_REQ_CMD_REG0_0_DIOC_MASK 0x4000000u

//! Field IE - Enable Interrupt for this port
#define PORT_REQ_CMD_REG0_0_IE_POS 29
//! Field IE - Enable Interrupt for this port
#define PORT_REQ_CMD_REG0_0_IE_MASK 0x20000000u
//! Constant NIL - not (default)
#define CONST_PORT_REQ_CMD_REG0_0_IE_NIL 0x0
//! Constant IE - Generate INT
#define CONST_PORT_REQ_CMD_REG0_0_IE_IE 0x1

//! Field DPHY - Destination address is Physical address
#define PORT_REQ_CMD_REG0_0_DPHY_POS 30
//! Field DPHY - Destination address is Physical address
#define PORT_REQ_CMD_REG0_0_DPHY_MASK 0x40000000u

//! Field SPHY - Source address is physical address
#define PORT_REQ_CMD_REG0_0_SPHY_POS 31
//! Field SPHY - Source address is physical address
#define PORT_REQ_CMD_REG0_0_SPHY_MASK 0x80000000u

//! @}

//! \defgroup PORT_REQ_CMD_REG1_0 Register PORT_REQ_CMD_REG1_0 - Request Command Source Buffer Pointer
//! @{

//! Register Offset (relative)
#define PORT_REQ_CMD_REG1_0 0x4

//! Register Reset Value
#define PORT_REQ_CMD_REG1_0_RST 0x00000000u

//! Field SRC_A - Source Address of data block
#define PORT_REQ_CMD_REG1_0_SRC_A_POS 0
//! Field SRC_A - Source Address of data block
#define PORT_REQ_CMD_REG1_0_SRC_A_MASK 0xFFFFFFFFu

//! @}

//! \defgroup PORT_REQ_CMD_REG2_0 Register PORT_REQ_CMD_REG2_0 - Request command register 2
//! @{

//! Register Offset (relative)
#define PORT_REQ_CMD_REG2_0 0x8

//! Register Reset Value
#define PORT_REQ_CMD_REG2_0_RST 0x00000000u

//! Field DMA_WORD0 - DMA Descriptor sideband DWORD 0
#define PORT_REQ_CMD_REG2_0_DMA_WORD0_POS 0
//! Field DMA_WORD0 - DMA Descriptor sideband DWORD 0
#define PORT_REQ_CMD_REG2_0_DMA_WORD0_MASK 0xFFFFFFFFu

//! @}

//! \defgroup PORT_REQ_CMD_REG3_0 Register PORT_REQ_CMD_REG3_0 - Request command register 3
//! @{

//! Register Offset (relative)
#define PORT_REQ_CMD_REG3_0 0xC

//! Register Reset Value
#define PORT_REQ_CMD_REG3_0_RST 0x00000000u

//! Field DMA_WORD1 - DMA Descriptor sideband DWORD1
#define PORT_REQ_CMD_REG3_0_DMA_WORD1_POS 0
//! Field DMA_WORD1 - DMA Descriptor sideband DWORD1
#define PORT_REQ_CMD_REG3_0_DMA_WORD1_MASK 0xFFFFFFFFu

//! @}

//! \defgroup PORT_REQ_CMD_REG4_0 Register PORT_REQ_CMD_REG4_0 - Request Command Packet Buffer Pointer
//! @{

//! Register Offset (relative)
#define PORT_REQ_CMD_REG4_0 0x10

//! Register Reset Value
#define PORT_REQ_CMD_REG4_0_RST 0x00000000u

//! Field PKT_A - Packet buffer address
#define PORT_REQ_CMD_REG4_0_PKT_A_POS 0
//! Field PKT_A - Packet buffer address
#define PORT_REQ_CMD_REG4_0_PKT_A_MASK 0xFFFFFFFFu

//! @}

//! \defgroup PORT_REQ_CMD_REG5_0 Register PORT_REQ_CMD_REG5_0 - Request Command register 5
//! @{

//! Register Offset (relative)
#define PORT_REQ_CMD_REG5_0 0x14

//! Register Reset Value
#define PORT_REQ_CMD_REG5_0_RST 0x80000000u

//! Field SEG_LEN - SEGMENT LENGTH
#define PORT_REQ_CMD_REG5_0_SEG_LEN_POS 0
//! Field SEG_LEN - SEGMENT LENGTH
#define PORT_REQ_CMD_REG5_0_SEG_LEN_MASK 0x1FFFu

//! Field JUMBO - Indication of Segment Length exceeds Standard Packet Size
#define PORT_REQ_CMD_REG5_0_JUMBO_POS 13
//! Field JUMBO - Indication of Segment Length exceeds Standard Packet Size
#define PORT_REQ_CMD_REG5_0_JUMBO_MASK 0x2000u
//! Constant STD - Segment Length not exceeds 1.5KB
#define CONST_PORT_REQ_CMD_REG5_0_JUMBO_STD 0x0
//! Constant JUMBO - Segment Length exceeds 1.5KB, up to 4KBytes
#define CONST_PORT_REQ_CMD_REG5_0_JUMBO_JUMBO 0x1

//! Field TCP - TCP Packet Indication
#define PORT_REQ_CMD_REG5_0_TCP_POS 14
//! Field TCP - TCP Packet Indication
#define PORT_REQ_CMD_REG5_0_TCP_MASK 0x4000u
//! Constant OTH - No TCP Packet
#define CONST_PORT_REQ_CMD_REG5_0_TCP_OTH 0x0
//! Constant TCP - Packet is TCP packet
#define CONST_PORT_REQ_CMD_REG5_0_TCP_TCP 0x1

//! Field TIRQ - Enable TSO Interrupt for the CMD port
#define PORT_REQ_CMD_REG5_0_TIRQ_POS 30
//! Field TIRQ - Enable TSO Interrupt for the CMD port
#define PORT_REQ_CMD_REG5_0_TIRQ_MASK 0x40000000u
//! Constant DIS - No interrupt after TSO
#define CONST_PORT_REQ_CMD_REG5_0_TIRQ_DIS 0x0
//! Constant EN - Enable Interrupt
#define CONST_PORT_REQ_CMD_REG5_0_TIRQ_EN 0x1

//! Field OWN - OWNER bit of the Command
#define PORT_REQ_CMD_REG5_0_OWN_POS 31
//! Field OWN - OWNER bit of the Command
#define PORT_REQ_CMD_REG5_0_OWN_MASK 0x80000000u
//! Constant TSO - TSO owns the command
#define CONST_PORT_REQ_CMD_REG5_0_OWN_TSO 0x0
//! Constant CPU - CPU Owns the command
#define CONST_PORT_REQ_CMD_REG5_0_OWN_CPU 0x1

//! @}

//! \defgroup PORT_REQ_CMD_REG0_1 Register PORT_REQ_CMD_REG0_1 - Port 0 request command register 0
//! @{

//! Register Offset (relative)
#define PORT_REQ_CMD_REG0_1 0x20

//! Register Reset Value
#define PORT_REQ_CMD_REG0_1_RST 0x00000000u

//! Field LEN - Data Block/Segment Length
#define PORT_REQ_CMD_REG0_1_LEN_POS 0
//! Field LEN - Data Block/Segment Length
#define PORT_REQ_CMD_REG0_1_LEN_MASK 0xFFFFFu

//! Field CHUNK_SIZ - Chunk size index
#define PORT_REQ_CMD_REG0_1_CHUNK_SIZ_POS 20
//! Field CHUNK_SIZ - Chunk size index
#define PORT_REQ_CMD_REG0_1_CHUNK_SIZ_MASK 0x700000u

//! Field SIOC - IOCU1 Address Translation for Source Address
#define PORT_REQ_CMD_REG0_1_SIOC_POS 23
//! Field SIOC - IOCU1 Address Translation for Source Address
#define PORT_REQ_CMD_REG0_1_SIOC_MASK 0x800000u
//! Constant NIL - no translation (default)
#define CONST_PORT_REQ_CMD_REG0_1_SIOC_NIL 0x0
//! Constant SIOC - Do translation
#define CONST_PORT_REQ_CMD_REG0_1_SIOC_SIOC 0x1

//! Field LAST - Last fragment of gathering
#define PORT_REQ_CMD_REG0_1_LAST_POS 24
//! Field LAST - Last fragment of gathering
#define PORT_REQ_CMD_REG0_1_LAST_MASK 0x1000000u
//! Constant NIL - not (default)
#define CONST_PORT_REQ_CMD_REG0_1_LAST_NIL 0x0
//! Constant LAST - Last fragment of gathering
#define CONST_PORT_REQ_CMD_REG0_1_LAST_LAST 0x1

//! Field G - Indication of gathering
#define PORT_REQ_CMD_REG0_1_G_POS 25
//! Field G - Indication of gathering
#define PORT_REQ_CMD_REG0_1_G_MASK 0x2000000u
//! Constant NIL - not (default)
#define CONST_PORT_REQ_CMD_REG0_1_G_NIL 0x0
//! Constant GATH - fragment of gathering
#define CONST_PORT_REQ_CMD_REG0_1_G_GATH 0x1

//! Field DIOC - Destination address needs to be translated into IOCU range
#define PORT_REQ_CMD_REG0_1_DIOC_POS 26
//! Field DIOC - Destination address needs to be translated into IOCU range
#define PORT_REQ_CMD_REG0_1_DIOC_MASK 0x4000000u

//! Field IE - Enable Interrupt for this port
#define PORT_REQ_CMD_REG0_1_IE_POS 29
//! Field IE - Enable Interrupt for this port
#define PORT_REQ_CMD_REG0_1_IE_MASK 0x20000000u
//! Constant NIL - not (default)
#define CONST_PORT_REQ_CMD_REG0_1_IE_NIL 0x0
//! Constant IE - Generate INT
#define CONST_PORT_REQ_CMD_REG0_1_IE_IE 0x1

//! Field DPHY - Destination address is Physical address
#define PORT_REQ_CMD_REG0_1_DPHY_POS 30
//! Field DPHY - Destination address is Physical address
#define PORT_REQ_CMD_REG0_1_DPHY_MASK 0x40000000u

//! Field SPHY - Source address is physical address
#define PORT_REQ_CMD_REG0_1_SPHY_POS 31
//! Field SPHY - Source address is physical address
#define PORT_REQ_CMD_REG0_1_SPHY_MASK 0x80000000u

//! @}

//! \defgroup PORT_REQ_CMD_REG1_1 Register PORT_REQ_CMD_REG1_1 - Request Command Source Buffer Pointer
//! @{

//! Register Offset (relative)
#define PORT_REQ_CMD_REG1_1 0x24

//! Register Reset Value
#define PORT_REQ_CMD_REG1_1_RST 0x00000000u

//! Field SRC_A - Source Address of data block
#define PORT_REQ_CMD_REG1_1_SRC_A_POS 0
//! Field SRC_A - Source Address of data block
#define PORT_REQ_CMD_REG1_1_SRC_A_MASK 0xFFFFFFFFu

//! @}

//! \defgroup PORT_REQ_CMD_REG2_1 Register PORT_REQ_CMD_REG2_1 - Request command register 2
//! @{

//! Register Offset (relative)
#define PORT_REQ_CMD_REG2_1 0x28

//! Register Reset Value
#define PORT_REQ_CMD_REG2_1_RST 0x00000000u

//! Field DMA_WORD0 - DMA Descriptor sideband DWORD 0
#define PORT_REQ_CMD_REG2_1_DMA_WORD0_POS 0
//! Field DMA_WORD0 - DMA Descriptor sideband DWORD 0
#define PORT_REQ_CMD_REG2_1_DMA_WORD0_MASK 0xFFFFFFFFu

//! @}

//! \defgroup PORT_REQ_CMD_REG3_1 Register PORT_REQ_CMD_REG3_1 - Request command register 3
//! @{

//! Register Offset (relative)
#define PORT_REQ_CMD_REG3_1 0x2C

//! Register Reset Value
#define PORT_REQ_CMD_REG3_1_RST 0x00000000u

//! Field DMA_WORD1 - DMA Descriptor sideband DWORD1
#define PORT_REQ_CMD_REG3_1_DMA_WORD1_POS 0
//! Field DMA_WORD1 - DMA Descriptor sideband DWORD1
#define PORT_REQ_CMD_REG3_1_DMA_WORD1_MASK 0xFFFFFFFFu

//! @}

//! \defgroup PORT_REQ_CMD_REG4_1 Register PORT_REQ_CMD_REG4_1 - Request Command Packet Buffer Pointer
//! @{

//! Register Offset (relative)
#define PORT_REQ_CMD_REG4_1 0x30

//! Register Reset Value
#define PORT_REQ_CMD_REG4_1_RST 0x00000000u

//! Field PKT_A - Packet buffer address
#define PORT_REQ_CMD_REG4_1_PKT_A_POS 0
//! Field PKT_A - Packet buffer address
#define PORT_REQ_CMD_REG4_1_PKT_A_MASK 0xFFFFFFFFu

//! @}

//! \defgroup PORT_REQ_CMD_REG5_1 Register PORT_REQ_CMD_REG5_1 - Request Command register 5
//! @{

//! Register Offset (relative)
#define PORT_REQ_CMD_REG5_1 0x34

//! Register Reset Value
#define PORT_REQ_CMD_REG5_1_RST 0x80000000u

//! Field SEG_LEN - SEGMENT LENGTH
#define PORT_REQ_CMD_REG5_1_SEG_LEN_POS 0
//! Field SEG_LEN - SEGMENT LENGTH
#define PORT_REQ_CMD_REG5_1_SEG_LEN_MASK 0x1FFFu

//! Field JUMBO - Indication of Segment Length exceeds Standard Packet Size
#define PORT_REQ_CMD_REG5_1_JUMBO_POS 13
//! Field JUMBO - Indication of Segment Length exceeds Standard Packet Size
#define PORT_REQ_CMD_REG5_1_JUMBO_MASK 0x2000u
//! Constant STD - Segment Length not exceeds 1.5KB
#define CONST_PORT_REQ_CMD_REG5_1_JUMBO_STD 0x0
//! Constant JUMBO - Segment Length exceeds 1.5KB, up to 4KBytes
#define CONST_PORT_REQ_CMD_REG5_1_JUMBO_JUMBO 0x1

//! Field TCP - TCP Packet Indication
#define PORT_REQ_CMD_REG5_1_TCP_POS 14
//! Field TCP - TCP Packet Indication
#define PORT_REQ_CMD_REG5_1_TCP_MASK 0x4000u
//! Constant OTH - No TCP Packet
#define CONST_PORT_REQ_CMD_REG5_1_TCP_OTH 0x0
//! Constant TCP - Packet is TCP packet
#define CONST_PORT_REQ_CMD_REG5_1_TCP_TCP 0x1

//! Field TIRQ - Enable TSO Interrupt for the CMD port
#define PORT_REQ_CMD_REG5_1_TIRQ_POS 30
//! Field TIRQ - Enable TSO Interrupt for the CMD port
#define PORT_REQ_CMD_REG5_1_TIRQ_MASK 0x40000000u
//! Constant DIS - No interrupt after TSO
#define CONST_PORT_REQ_CMD_REG5_1_TIRQ_DIS 0x0
//! Constant EN - Enable Interrupt
#define CONST_PORT_REQ_CMD_REG5_1_TIRQ_EN 0x1

//! Field OWN - OWNER bit of the Command
#define PORT_REQ_CMD_REG5_1_OWN_POS 31
//! Field OWN - OWNER bit of the Command
#define PORT_REQ_CMD_REG5_1_OWN_MASK 0x80000000u
//! Constant TSO - TSO owns the command
#define CONST_PORT_REQ_CMD_REG5_1_OWN_TSO 0x0
//! Constant CPU - CPU Owns the command
#define CONST_PORT_REQ_CMD_REG5_1_OWN_CPU 0x1

//! @}

//! \defgroup PORT_REQ_CMD_REG0_2 Register PORT_REQ_CMD_REG0_2 - Port 0 request command register 0
//! @{

//! Register Offset (relative)
#define PORT_REQ_CMD_REG0_2 0x40

//! Register Reset Value
#define PORT_REQ_CMD_REG0_2_RST 0x00000000u

//! Field LEN - Data Block/Segment Length
#define PORT_REQ_CMD_REG0_2_LEN_POS 0
//! Field LEN - Data Block/Segment Length
#define PORT_REQ_CMD_REG0_2_LEN_MASK 0xFFFFFu

//! Field CHUNK_SIZ - Chunk size index
#define PORT_REQ_CMD_REG0_2_CHUNK_SIZ_POS 20
//! Field CHUNK_SIZ - Chunk size index
#define PORT_REQ_CMD_REG0_2_CHUNK_SIZ_MASK 0x700000u

//! Field SIOC - IOCU1 Address Translation for Source Address
#define PORT_REQ_CMD_REG0_2_SIOC_POS 23
//! Field SIOC - IOCU1 Address Translation for Source Address
#define PORT_REQ_CMD_REG0_2_SIOC_MASK 0x800000u
//! Constant NIL - no translation (default)
#define CONST_PORT_REQ_CMD_REG0_2_SIOC_NIL 0x0
//! Constant SIOC - Do translation
#define CONST_PORT_REQ_CMD_REG0_2_SIOC_SIOC 0x1

//! Field LAST - Last fragment of gathering
#define PORT_REQ_CMD_REG0_2_LAST_POS 24
//! Field LAST - Last fragment of gathering
#define PORT_REQ_CMD_REG0_2_LAST_MASK 0x1000000u
//! Constant NIL - not (default)
#define CONST_PORT_REQ_CMD_REG0_2_LAST_NIL 0x0
//! Constant LAST - Last fragment of gathering
#define CONST_PORT_REQ_CMD_REG0_2_LAST_LAST 0x1

//! Field G - Indication of gathering
#define PORT_REQ_CMD_REG0_2_G_POS 25
//! Field G - Indication of gathering
#define PORT_REQ_CMD_REG0_2_G_MASK 0x2000000u
//! Constant NIL - not (default)
#define CONST_PORT_REQ_CMD_REG0_2_G_NIL 0x0
//! Constant GATH - fragment of gathering
#define CONST_PORT_REQ_CMD_REG0_2_G_GATH 0x1

//! Field DIOC - Destination address needs to be translated into IOCU range
#define PORT_REQ_CMD_REG0_2_DIOC_POS 26
//! Field DIOC - Destination address needs to be translated into IOCU range
#define PORT_REQ_CMD_REG0_2_DIOC_MASK 0x4000000u

//! Field IE - Enable Interrupt for this port
#define PORT_REQ_CMD_REG0_2_IE_POS 29
//! Field IE - Enable Interrupt for this port
#define PORT_REQ_CMD_REG0_2_IE_MASK 0x20000000u
//! Constant NIL - not (default)
#define CONST_PORT_REQ_CMD_REG0_2_IE_NIL 0x0
//! Constant IE - Generate INT
#define CONST_PORT_REQ_CMD_REG0_2_IE_IE 0x1

//! Field DPHY - Destination address is Physical address
#define PORT_REQ_CMD_REG0_2_DPHY_POS 30
//! Field DPHY - Destination address is Physical address
#define PORT_REQ_CMD_REG0_2_DPHY_MASK 0x40000000u

//! Field SPHY - Source address is physical address
#define PORT_REQ_CMD_REG0_2_SPHY_POS 31
//! Field SPHY - Source address is physical address
#define PORT_REQ_CMD_REG0_2_SPHY_MASK 0x80000000u

//! @}

//! \defgroup PORT_REQ_CMD_REG1_2 Register PORT_REQ_CMD_REG1_2 - Request Command Source Buffer Pointer
//! @{

//! Register Offset (relative)
#define PORT_REQ_CMD_REG1_2 0x44

//! Register Reset Value
#define PORT_REQ_CMD_REG1_2_RST 0x00000000u

//! Field SRC_A - Source Address of data block
#define PORT_REQ_CMD_REG1_2_SRC_A_POS 0
//! Field SRC_A - Source Address of data block
#define PORT_REQ_CMD_REG1_2_SRC_A_MASK 0xFFFFFFFFu

//! @}

//! \defgroup PORT_REQ_CMD_REG2_2 Register PORT_REQ_CMD_REG2_2 - Request command register 2
//! @{

//! Register Offset (relative)
#define PORT_REQ_CMD_REG2_2 0x48

//! Register Reset Value
#define PORT_REQ_CMD_REG2_2_RST 0x00000000u

//! Field DMA_WORD0 - DMA Descriptor sideband DWORD 0
#define PORT_REQ_CMD_REG2_2_DMA_WORD0_POS 0
//! Field DMA_WORD0 - DMA Descriptor sideband DWORD 0
#define PORT_REQ_CMD_REG2_2_DMA_WORD0_MASK 0xFFFFFFFFu

//! @}

//! \defgroup PORT_REQ_CMD_REG3_2 Register PORT_REQ_CMD_REG3_2 - Request command register 3
//! @{

//! Register Offset (relative)
#define PORT_REQ_CMD_REG3_2 0x4C

//! Register Reset Value
#define PORT_REQ_CMD_REG3_2_RST 0x00000000u

//! Field DMA_WORD1 - DMA Descriptor sideband DWORD1
#define PORT_REQ_CMD_REG3_2_DMA_WORD1_POS 0
//! Field DMA_WORD1 - DMA Descriptor sideband DWORD1
#define PORT_REQ_CMD_REG3_2_DMA_WORD1_MASK 0xFFFFFFFFu

//! @}

//! \defgroup PORT_REQ_CMD_REG4_2 Register PORT_REQ_CMD_REG4_2 - Request Command Packet Buffer Pointer
//! @{

//! Register Offset (relative)
#define PORT_REQ_CMD_REG4_2 0x50

//! Register Reset Value
#define PORT_REQ_CMD_REG4_2_RST 0x00000000u

//! Field PKT_A - Packet buffer address
#define PORT_REQ_CMD_REG4_2_PKT_A_POS 0
//! Field PKT_A - Packet buffer address
#define PORT_REQ_CMD_REG4_2_PKT_A_MASK 0xFFFFFFFFu

//! @}

//! \defgroup PORT_REQ_CMD_REG5_2 Register PORT_REQ_CMD_REG5_2 - Request Command register 5
//! @{

//! Register Offset (relative)
#define PORT_REQ_CMD_REG5_2 0x54

//! Register Reset Value
#define PORT_REQ_CMD_REG5_2_RST 0x80000000u

//! Field SEG_LEN - SEGMENT LENGTH
#define PORT_REQ_CMD_REG5_2_SEG_LEN_POS 0
//! Field SEG_LEN - SEGMENT LENGTH
#define PORT_REQ_CMD_REG5_2_SEG_LEN_MASK 0x1FFFu

//! Field JUMBO - Indication of Segment Length exceeds Standard Packet Size
#define PORT_REQ_CMD_REG5_2_JUMBO_POS 13
//! Field JUMBO - Indication of Segment Length exceeds Standard Packet Size
#define PORT_REQ_CMD_REG5_2_JUMBO_MASK 0x2000u
//! Constant STD - Segment Length not exceeds 1.5KB
#define CONST_PORT_REQ_CMD_REG5_2_JUMBO_STD 0x0
//! Constant JUMBO - Segment Length exceeds 1.5KB, up to 4KBytes
#define CONST_PORT_REQ_CMD_REG5_2_JUMBO_JUMBO 0x1

//! Field TCP - TCP Packet Indication
#define PORT_REQ_CMD_REG5_2_TCP_POS 14
//! Field TCP - TCP Packet Indication
#define PORT_REQ_CMD_REG5_2_TCP_MASK 0x4000u
//! Constant OTH - No TCP Packet
#define CONST_PORT_REQ_CMD_REG5_2_TCP_OTH 0x0
//! Constant TCP - Packet is TCP packet
#define CONST_PORT_REQ_CMD_REG5_2_TCP_TCP 0x1

//! Field TIRQ - Enable TSO Interrupt for the CMD port
#define PORT_REQ_CMD_REG5_2_TIRQ_POS 30
//! Field TIRQ - Enable TSO Interrupt for the CMD port
#define PORT_REQ_CMD_REG5_2_TIRQ_MASK 0x40000000u
//! Constant DIS - No interrupt after TSO
#define CONST_PORT_REQ_CMD_REG5_2_TIRQ_DIS 0x0
//! Constant EN - Enable Interrupt
#define CONST_PORT_REQ_CMD_REG5_2_TIRQ_EN 0x1

//! Field OWN - OWNER bit of the Command
#define PORT_REQ_CMD_REG5_2_OWN_POS 31
//! Field OWN - OWNER bit of the Command
#define PORT_REQ_CMD_REG5_2_OWN_MASK 0x80000000u
//! Constant TSO - TSO owns the command
#define CONST_PORT_REQ_CMD_REG5_2_OWN_TSO 0x0
//! Constant CPU - CPU Owns the command
#define CONST_PORT_REQ_CMD_REG5_2_OWN_CPU 0x1

//! @}

//! \defgroup PORT_REQ_CMD_REG0_3 Register PORT_REQ_CMD_REG0_3 - Port 0 request command register 0
//! @{

//! Register Offset (relative)
#define PORT_REQ_CMD_REG0_3 0x60

//! Register Reset Value
#define PORT_REQ_CMD_REG0_3_RST 0x00000000u

//! Field LEN - Data Block/Segment Length
#define PORT_REQ_CMD_REG0_3_LEN_POS 0
//! Field LEN - Data Block/Segment Length
#define PORT_REQ_CMD_REG0_3_LEN_MASK 0xFFFFFu

//! Field CHUNK_SIZ - Chunk size index
#define PORT_REQ_CMD_REG0_3_CHUNK_SIZ_POS 20
//! Field CHUNK_SIZ - Chunk size index
#define PORT_REQ_CMD_REG0_3_CHUNK_SIZ_MASK 0x700000u

//! Field SIOC - IOCU1 Address Translation for Source Address
#define PORT_REQ_CMD_REG0_3_SIOC_POS 23
//! Field SIOC - IOCU1 Address Translation for Source Address
#define PORT_REQ_CMD_REG0_3_SIOC_MASK 0x800000u
//! Constant NIL - no translation (default)
#define CONST_PORT_REQ_CMD_REG0_3_SIOC_NIL 0x0
//! Constant SIOC - Do translation
#define CONST_PORT_REQ_CMD_REG0_3_SIOC_SIOC 0x1

//! Field LAST - Last fragment of gathering
#define PORT_REQ_CMD_REG0_3_LAST_POS 24
//! Field LAST - Last fragment of gathering
#define PORT_REQ_CMD_REG0_3_LAST_MASK 0x1000000u
//! Constant NIL - not (default)
#define CONST_PORT_REQ_CMD_REG0_3_LAST_NIL 0x0
//! Constant LAST - Last fragment of gathering
#define CONST_PORT_REQ_CMD_REG0_3_LAST_LAST 0x1

//! Field G - Indication of gathering
#define PORT_REQ_CMD_REG0_3_G_POS 25
//! Field G - Indication of gathering
#define PORT_REQ_CMD_REG0_3_G_MASK 0x2000000u
//! Constant NIL - not (default)
#define CONST_PORT_REQ_CMD_REG0_3_G_NIL 0x0
//! Constant GATH - fragment of gathering
#define CONST_PORT_REQ_CMD_REG0_3_G_GATH 0x1

//! Field DIOC - Destination address needs to be translated into IOCU range
#define PORT_REQ_CMD_REG0_3_DIOC_POS 26
//! Field DIOC - Destination address needs to be translated into IOCU range
#define PORT_REQ_CMD_REG0_3_DIOC_MASK 0x4000000u

//! Field IE - Enable Interrupt for this port
#define PORT_REQ_CMD_REG0_3_IE_POS 29
//! Field IE - Enable Interrupt for this port
#define PORT_REQ_CMD_REG0_3_IE_MASK 0x20000000u
//! Constant NIL - not (default)
#define CONST_PORT_REQ_CMD_REG0_3_IE_NIL 0x0
//! Constant IE - Generate INT
#define CONST_PORT_REQ_CMD_REG0_3_IE_IE 0x1

//! Field DPHY - Destination address is Physical address
#define PORT_REQ_CMD_REG0_3_DPHY_POS 30
//! Field DPHY - Destination address is Physical address
#define PORT_REQ_CMD_REG0_3_DPHY_MASK 0x40000000u

//! Field SPHY - Source address is physical address
#define PORT_REQ_CMD_REG0_3_SPHY_POS 31
//! Field SPHY - Source address is physical address
#define PORT_REQ_CMD_REG0_3_SPHY_MASK 0x80000000u

//! @}

//! \defgroup PORT_REQ_CMD_REG1_3 Register PORT_REQ_CMD_REG1_3 - Request Command Source Buffer Pointer
//! @{

//! Register Offset (relative)
#define PORT_REQ_CMD_REG1_3 0x64

//! Register Reset Value
#define PORT_REQ_CMD_REG1_3_RST 0x00000000u

//! Field SRC_A - Source Address of data block
#define PORT_REQ_CMD_REG1_3_SRC_A_POS 0
//! Field SRC_A - Source Address of data block
#define PORT_REQ_CMD_REG1_3_SRC_A_MASK 0xFFFFFFFFu

//! @}

//! \defgroup PORT_REQ_CMD_REG2_3 Register PORT_REQ_CMD_REG2_3 - Request command register 2
//! @{

//! Register Offset (relative)
#define PORT_REQ_CMD_REG2_3 0x68

//! Register Reset Value
#define PORT_REQ_CMD_REG2_3_RST 0x00000000u

//! Field DMA_WORD0 - DMA Descriptor sideband DWORD 0
#define PORT_REQ_CMD_REG2_3_DMA_WORD0_POS 0
//! Field DMA_WORD0 - DMA Descriptor sideband DWORD 0
#define PORT_REQ_CMD_REG2_3_DMA_WORD0_MASK 0xFFFFFFFFu

//! @}

//! \defgroup PORT_REQ_CMD_REG3_3 Register PORT_REQ_CMD_REG3_3 - Request command register 3
//! @{

//! Register Offset (relative)
#define PORT_REQ_CMD_REG3_3 0x6C

//! Register Reset Value
#define PORT_REQ_CMD_REG3_3_RST 0x00000000u

//! Field DMA_WORD1 - DMA Descriptor sideband DWORD1
#define PORT_REQ_CMD_REG3_3_DMA_WORD1_POS 0
//! Field DMA_WORD1 - DMA Descriptor sideband DWORD1
#define PORT_REQ_CMD_REG3_3_DMA_WORD1_MASK 0xFFFFFFFFu

//! @}

//! \defgroup PORT_REQ_CMD_REG4_3 Register PORT_REQ_CMD_REG4_3 - Request Command Packet Buffer Pointer
//! @{

//! Register Offset (relative)
#define PORT_REQ_CMD_REG4_3 0x70

//! Register Reset Value
#define PORT_REQ_CMD_REG4_3_RST 0x00000000u

//! Field PKT_A - Packet buffer address
#define PORT_REQ_CMD_REG4_3_PKT_A_POS 0
//! Field PKT_A - Packet buffer address
#define PORT_REQ_CMD_REG4_3_PKT_A_MASK 0xFFFFFFFFu

//! @}

//! \defgroup PORT_REQ_CMD_REG5_3 Register PORT_REQ_CMD_REG5_3 - Request Command register 5
//! @{

//! Register Offset (relative)
#define PORT_REQ_CMD_REG5_3 0x74

//! Register Reset Value
#define PORT_REQ_CMD_REG5_3_RST 0x80000000u

//! Field SEG_LEN - SEGMENT LENGTH
#define PORT_REQ_CMD_REG5_3_SEG_LEN_POS 0
//! Field SEG_LEN - SEGMENT LENGTH
#define PORT_REQ_CMD_REG5_3_SEG_LEN_MASK 0x1FFFu

//! Field JUMBO - Indication of Segment Length exceeds Standard Packet Size
#define PORT_REQ_CMD_REG5_3_JUMBO_POS 13
//! Field JUMBO - Indication of Segment Length exceeds Standard Packet Size
#define PORT_REQ_CMD_REG5_3_JUMBO_MASK 0x2000u
//! Constant STD - Segment Length not exceeds 1.5KB
#define CONST_PORT_REQ_CMD_REG5_3_JUMBO_STD 0x0
//! Constant JUMBO - Segment Length exceeds 1.5KB, up to 4KBytes
#define CONST_PORT_REQ_CMD_REG5_3_JUMBO_JUMBO 0x1

//! Field TCP - TCP Packet Indication
#define PORT_REQ_CMD_REG5_3_TCP_POS 14
//! Field TCP - TCP Packet Indication
#define PORT_REQ_CMD_REG5_3_TCP_MASK 0x4000u
//! Constant OTH - No TCP Packet
#define CONST_PORT_REQ_CMD_REG5_3_TCP_OTH 0x0
//! Constant TCP - Packet is TCP packet
#define CONST_PORT_REQ_CMD_REG5_3_TCP_TCP 0x1

//! Field TIRQ - Enable TSO Interrupt for the CMD port
#define PORT_REQ_CMD_REG5_3_TIRQ_POS 30
//! Field TIRQ - Enable TSO Interrupt for the CMD port
#define PORT_REQ_CMD_REG5_3_TIRQ_MASK 0x40000000u
//! Constant DIS - No interrupt after TSO
#define CONST_PORT_REQ_CMD_REG5_3_TIRQ_DIS 0x0
//! Constant EN - Enable Interrupt
#define CONST_PORT_REQ_CMD_REG5_3_TIRQ_EN 0x1

//! Field OWN - OWNER bit of the Command
#define PORT_REQ_CMD_REG5_3_OWN_POS 31
//! Field OWN - OWNER bit of the Command
#define PORT_REQ_CMD_REG5_3_OWN_MASK 0x80000000u
//! Constant TSO - TSO owns the command
#define CONST_PORT_REQ_CMD_REG5_3_OWN_TSO 0x0
//! Constant CPU - CPU Owns the command
#define CONST_PORT_REQ_CMD_REG5_3_OWN_CPU 0x1

//! @}

//! \defgroup PORT_RES_REG0_0 Register PORT_RES_REG0_0 - Response register 0
//! @{

//! Register Offset (relative)
#define PORT_RES_REG0_0 0x100

//! Register Reset Value
#define PORT_RES_REG0_0_RST 0x00000000u

//! Field PKT_A - Packet buffer address
#define PORT_RES_REG0_0_PKT_A_POS 0
//! Field PKT_A - Packet buffer address
#define PORT_RES_REG0_0_PKT_A_MASK 0xFFFFFFFFu

//! @}

//! \defgroup PORT_RES_REG1_0 Register PORT_RES_REG1_0 - Response register 1
//! @{

//! Register Offset (relative)
#define PORT_RES_REG1_0 0x104

//! Register Reset Value
#define PORT_RES_REG1_0_RST 0x00000000u

//! Field SEG_NR - Number of segments
#define PORT_RES_REG1_0_SEG_NR_POS 0
//! Field SEG_NR - Number of segments
#define PORT_RES_REG1_0_SEG_NR_MASK 0x1Fu

//! Field CONTEXT - Total Length of all segments
#define PORT_RES_REG1_0_CONTEXT_POS 6
//! Field CONTEXT - Total Length of all segments
#define PORT_RES_REG1_0_CONTEXT_MASK 0x7FFFC0u

//! Field ABORT - Abort by SW
#define PORT_RES_REG1_0_ABORT_POS 29
//! Field ABORT - Abort by SW
#define PORT_RES_REG1_0_ABORT_MASK 0x20000000u

//! Field ERR - TSO Error
#define PORT_RES_REG1_0_ERR_POS 30
//! Field ERR - TSO Error
#define PORT_RES_REG1_0_ERR_MASK 0x40000000u
//! Constant NIL - no error
#define CONST_PORT_RES_REG1_0_ERR_NIL 0x0
//! Constant ERR - TSO is error
#define CONST_PORT_RES_REG1_0_ERR_ERR 0x1

//! Field DONE - TSO done
#define PORT_RES_REG1_0_DONE_POS 31
//! Field DONE - TSO done
#define PORT_RES_REG1_0_DONE_MASK 0x80000000u
//! Constant NIL - not done.
#define CONST_PORT_RES_REG1_0_DONE_NIL 0x0
//! Constant DONE - TSO is completed
#define CONST_PORT_RES_REG1_0_DONE_DONE 0x1

//! @}

//! \defgroup PORT_RES_REG0_1 Register PORT_RES_REG0_1 - Response register 0
//! @{

//! Register Offset (relative)
#define PORT_RES_REG0_1 0x120

//! Register Reset Value
#define PORT_RES_REG0_1_RST 0x00000000u

//! Field PKT_A - Packet buffer address
#define PORT_RES_REG0_1_PKT_A_POS 0
//! Field PKT_A - Packet buffer address
#define PORT_RES_REG0_1_PKT_A_MASK 0xFFFFFFFFu

//! @}

//! \defgroup PORT_RES_REG1_1 Register PORT_RES_REG1_1 - Response register 1
//! @{

//! Register Offset (relative)
#define PORT_RES_REG1_1 0x124

//! Register Reset Value
#define PORT_RES_REG1_1_RST 0x00000000u

//! Field SEG_NR - Number of segments
#define PORT_RES_REG1_1_SEG_NR_POS 0
//! Field SEG_NR - Number of segments
#define PORT_RES_REG1_1_SEG_NR_MASK 0x1Fu

//! Field CONTEXT - Total Length of all segments
#define PORT_RES_REG1_1_CONTEXT_POS 6
//! Field CONTEXT - Total Length of all segments
#define PORT_RES_REG1_1_CONTEXT_MASK 0x7FFFC0u

//! Field ABORT - Abort by SW
#define PORT_RES_REG1_1_ABORT_POS 29
//! Field ABORT - Abort by SW
#define PORT_RES_REG1_1_ABORT_MASK 0x20000000u

//! Field ERR - TSO Error
#define PORT_RES_REG1_1_ERR_POS 30
//! Field ERR - TSO Error
#define PORT_RES_REG1_1_ERR_MASK 0x40000000u
//! Constant NIL - no error
#define CONST_PORT_RES_REG1_1_ERR_NIL 0x0
//! Constant ERR - TSO is error
#define CONST_PORT_RES_REG1_1_ERR_ERR 0x1

//! Field DONE - TSO done
#define PORT_RES_REG1_1_DONE_POS 31
//! Field DONE - TSO done
#define PORT_RES_REG1_1_DONE_MASK 0x80000000u
//! Constant NIL - not done.
#define CONST_PORT_RES_REG1_1_DONE_NIL 0x0
//! Constant DONE - TSO is completed
#define CONST_PORT_RES_REG1_1_DONE_DONE 0x1

//! @}

//! \defgroup PORT_RES_REG0_2 Register PORT_RES_REG0_2 - Response register 0
//! @{

//! Register Offset (relative)
#define PORT_RES_REG0_2 0x140

//! Register Reset Value
#define PORT_RES_REG0_2_RST 0x00000000u

//! Field PKT_A - Packet buffer address
#define PORT_RES_REG0_2_PKT_A_POS 0
//! Field PKT_A - Packet buffer address
#define PORT_RES_REG0_2_PKT_A_MASK 0xFFFFFFFFu

//! @}

//! \defgroup PORT_RES_REG1_2 Register PORT_RES_REG1_2 - Response register 1
//! @{

//! Register Offset (relative)
#define PORT_RES_REG1_2 0x144

//! Register Reset Value
#define PORT_RES_REG1_2_RST 0x00000000u

//! Field SEG_NR - Number of segments
#define PORT_RES_REG1_2_SEG_NR_POS 0
//! Field SEG_NR - Number of segments
#define PORT_RES_REG1_2_SEG_NR_MASK 0x1Fu

//! Field CONTEXT - Total Length of all segments
#define PORT_RES_REG1_2_CONTEXT_POS 6
//! Field CONTEXT - Total Length of all segments
#define PORT_RES_REG1_2_CONTEXT_MASK 0x7FFFC0u

//! Field ABORT - Abort by SW
#define PORT_RES_REG1_2_ABORT_POS 29
//! Field ABORT - Abort by SW
#define PORT_RES_REG1_2_ABORT_MASK 0x20000000u

//! Field ERR - TSO Error
#define PORT_RES_REG1_2_ERR_POS 30
//! Field ERR - TSO Error
#define PORT_RES_REG1_2_ERR_MASK 0x40000000u
//! Constant NIL - no error
#define CONST_PORT_RES_REG1_2_ERR_NIL 0x0
//! Constant ERR - TSO is error
#define CONST_PORT_RES_REG1_2_ERR_ERR 0x1

//! Field DONE - TSO done
#define PORT_RES_REG1_2_DONE_POS 31
//! Field DONE - TSO done
#define PORT_RES_REG1_2_DONE_MASK 0x80000000u
//! Constant NIL - not done.
#define CONST_PORT_RES_REG1_2_DONE_NIL 0x0
//! Constant DONE - TSO is completed
#define CONST_PORT_RES_REG1_2_DONE_DONE 0x1

//! @}

//! \defgroup PORT_RES_REG0_3 Register PORT_RES_REG0_3 - Response register 0
//! @{

//! Register Offset (relative)
#define PORT_RES_REG0_3 0x160

//! Register Reset Value
#define PORT_RES_REG0_3_RST 0x00000000u

//! Field PKT_A - Packet buffer address
#define PORT_RES_REG0_3_PKT_A_POS 0
//! Field PKT_A - Packet buffer address
#define PORT_RES_REG0_3_PKT_A_MASK 0xFFFFFFFFu

//! @}

//! \defgroup PORT_RES_REG1_3 Register PORT_RES_REG1_3 - Response register 1
//! @{

//! Register Offset (relative)
#define PORT_RES_REG1_3 0x164

//! Register Reset Value
#define PORT_RES_REG1_3_RST 0x00000000u

//! Field SEG_NR - Number of segments
#define PORT_RES_REG1_3_SEG_NR_POS 0
//! Field SEG_NR - Number of segments
#define PORT_RES_REG1_3_SEG_NR_MASK 0x1Fu

//! Field CONTEXT - Total Length of all segments
#define PORT_RES_REG1_3_CONTEXT_POS 6
//! Field CONTEXT - Total Length of all segments
#define PORT_RES_REG1_3_CONTEXT_MASK 0x7FFFC0u

//! Field ABORT - Abort by SW
#define PORT_RES_REG1_3_ABORT_POS 29
//! Field ABORT - Abort by SW
#define PORT_RES_REG1_3_ABORT_MASK 0x20000000u

//! Field ERR - TSO Error
#define PORT_RES_REG1_3_ERR_POS 30
//! Field ERR - TSO Error
#define PORT_RES_REG1_3_ERR_MASK 0x40000000u
//! Constant NIL - no error
#define CONST_PORT_RES_REG1_3_ERR_NIL 0x0
//! Constant ERR - TSO is error
#define CONST_PORT_RES_REG1_3_ERR_ERR 0x1

//! Field DONE - TSO done
#define PORT_RES_REG1_3_DONE_POS 31
//! Field DONE - TSO done
#define PORT_RES_REG1_3_DONE_MASK 0x80000000u
//! Constant NIL - not done.
#define CONST_PORT_RES_REG1_3_DONE_NIL 0x0
//! Constant DONE - TSO is completed
#define CONST_PORT_RES_REG1_3_DONE_DONE 0x1

//! @}

//! \defgroup TSO_GCTRL Register TSO_GCTRL - Global Control Register
//! @{

//! Register Offset (relative)
#define TSO_GCTRL 0x200

//! Register Reset Value
#define TSO_GCTRL_RST 0x00040000u

//! Field ENABLE - Enable ToE
#define TSO_GCTRL_ENABLE_POS 0
//! Field ENABLE - Enable ToE
#define TSO_GCTRL_ENABLE_MASK 0x1u
//! Constant DIS - Disable ToE
#define CONST_TSO_GCTRL_ENABLE_DIS 0x0
//! Constant EN - Enable ToE
#define CONST_TSO_GCTRL_ENABLE_EN 0x1

//! Field ENA_P0 - Enable TSO for PORT0
#define TSO_GCTRL_ENA_P0_POS 1
//! Field ENA_P0 - Enable TSO for PORT0
#define TSO_GCTRL_ENA_P0_MASK 0x2u
//! Constant DIS - Disable TSO
#define CONST_TSO_GCTRL_ENA_P0_DIS 0x0
//! Constant EN - Enable TSO
#define CONST_TSO_GCTRL_ENA_P0_EN 0x1

//! Field ENA_P1 - Enable TSO for PORT1
#define TSO_GCTRL_ENA_P1_POS 2
//! Field ENA_P1 - Enable TSO for PORT1
#define TSO_GCTRL_ENA_P1_MASK 0x4u
//! Constant DIS - Disable TSO
#define CONST_TSO_GCTRL_ENA_P1_DIS 0x0
//! Constant EN - Enable TSO
#define CONST_TSO_GCTRL_ENA_P1_EN 0x1

//! Field ENA_P2 - Enable TSO for PORT2
#define TSO_GCTRL_ENA_P2_POS 3
//! Field ENA_P2 - Enable TSO for PORT2
#define TSO_GCTRL_ENA_P2_MASK 0x8u
//! Constant DIS - Disable TSO
#define CONST_TSO_GCTRL_ENA_P2_DIS 0x0
//! Constant EN - Enable TSO
#define CONST_TSO_GCTRL_ENA_P2_EN 0x1

//! Field ENA_P3 - Enable TSO for PORT3
#define TSO_GCTRL_ENA_P3_POS 4
//! Field ENA_P3 - Enable TSO for PORT3
#define TSO_GCTRL_ENA_P3_MASK 0x10u
//! Constant DIS - Disable TSO
#define CONST_TSO_GCTRL_ENA_P3_DIS 0x0
//! Constant EN - Enable TSO
#define CONST_TSO_GCTRL_ENA_P3_EN 0x1

//! Field DEBUG_STEP_TSO - Step debugging
#define TSO_GCTRL_DEBUG_STEP_TSO_POS 5
//! Field DEBUG_STEP_TSO - Step debugging
#define TSO_GCTRL_DEBUG_STEP_TSO_MASK 0x20u
//! Constant OVER - step complete
#define CONST_TSO_GCTRL_DEBUG_STEP_TSO_OVER 0x0
//! Constant STEP - step ahead
#define CONST_TSO_GCTRL_DEBUG_STEP_TSO_STEP 0x1

//! Field DEBUG_STEP_LRO - Step debugging
#define TSO_GCTRL_DEBUG_STEP_LRO_POS 6
//! Field DEBUG_STEP_LRO - Step debugging
#define TSO_GCTRL_DEBUG_STEP_LRO_MASK 0x40u
//! Constant OVER - step complete
#define CONST_TSO_GCTRL_DEBUG_STEP_LRO_OVER 0x0
//! Constant STEP - step ahead
#define CONST_TSO_GCTRL_DEBUG_STEP_LRO_STEP 0x1

//! Field LRO0_EN - ENABLE LRO 0
#define TSO_GCTRL_LRO0_EN_POS 8
//! Field LRO0_EN - ENABLE LRO 0
#define TSO_GCTRL_LRO0_EN_MASK 0x100u
//! Constant DISU - DISABLE LRO
#define CONST_TSO_GCTRL_LRO0_EN_DISU 0x0
//! Constant ENU - ENABLE LRO
#define CONST_TSO_GCTRL_LRO0_EN_ENU 0x1

//! Field LRO1_EN - ENABLE LRO 1
#define TSO_GCTRL_LRO1_EN_POS 9
//! Field LRO1_EN - ENABLE LRO 1
#define TSO_GCTRL_LRO1_EN_MASK 0x200u
//! Constant DISU - DISABLE LRO
#define CONST_TSO_GCTRL_LRO1_EN_DISU 0x0
//! Constant ENU - ENABLE LRO
#define CONST_TSO_GCTRL_LRO1_EN_ENU 0x1

//! Field LRO2_EN - ENABLE LRO 2
#define TSO_GCTRL_LRO2_EN_POS 10
//! Field LRO2_EN - ENABLE LRO 2
#define TSO_GCTRL_LRO2_EN_MASK 0x400u
//! Constant DISU - DISABLE LRO
#define CONST_TSO_GCTRL_LRO2_EN_DISU 0x0
//! Constant ENU - ENABLE LRO
#define CONST_TSO_GCTRL_LRO2_EN_ENU 0x1

//! Field LRO3_EN - ENABLE LRO 3
#define TSO_GCTRL_LRO3_EN_POS 11
//! Field LRO3_EN - ENABLE LRO 3
#define TSO_GCTRL_LRO3_EN_MASK 0x800u
//! Constant DISU - DISABLE LRO
#define CONST_TSO_GCTRL_LRO3_EN_DISU 0x0
//! Constant ENU - ENABLE LRO
#define CONST_TSO_GCTRL_LRO3_EN_ENU 0x1

//! Field LRO4_EN - ENABLE LRO 4
#define TSO_GCTRL_LRO4_EN_POS 12
//! Field LRO4_EN - ENABLE LRO 4
#define TSO_GCTRL_LRO4_EN_MASK 0x1000u
//! Constant DISU - DISABLE LRO
#define CONST_TSO_GCTRL_LRO4_EN_DISU 0x0
//! Constant ENU - ENABLE LRO
#define CONST_TSO_GCTRL_LRO4_EN_ENU 0x1

//! Field LRO5_EN - ENABLE LRO 5
#define TSO_GCTRL_LRO5_EN_POS 13
//! Field LRO5_EN - ENABLE LRO 5
#define TSO_GCTRL_LRO5_EN_MASK 0x2000u
//! Constant DISU - DISABLE LRO
#define CONST_TSO_GCTRL_LRO5_EN_DISU 0x0
//! Constant ENU - ENABLE LRO
#define CONST_TSO_GCTRL_LRO5_EN_ENU 0x1

//! Field LRO6_EN - ENABLE LRO 6
#define TSO_GCTRL_LRO6_EN_POS 14
//! Field LRO6_EN - ENABLE LRO 6
#define TSO_GCTRL_LRO6_EN_MASK 0x4000u
//! Constant DISU - DISABLE LRO
#define CONST_TSO_GCTRL_LRO6_EN_DISU 0x0
//! Constant ENU - ENABLE LRO
#define CONST_TSO_GCTRL_LRO6_EN_ENU 0x1

//! Field LRO7_EN - ENABLE LRO 7
#define TSO_GCTRL_LRO7_EN_POS 15
//! Field LRO7_EN - ENABLE LRO 7
#define TSO_GCTRL_LRO7_EN_MASK 0x8000u
//! Constant DISU - DISABLE LRO
#define CONST_TSO_GCTRL_LRO7_EN_DISU 0x0
//! Constant ENU - ENABLE LRO
#define CONST_TSO_GCTRL_LRO7_EN_ENU 0x1

//! Field TOE_RST - Reset TSO and LRO
#define TSO_GCTRL_TOE_RST_POS 16
//! Field TOE_RST - Reset TSO and LRO
#define TSO_GCTRL_TOE_RST_MASK 0x10000u
//! Constant TOENRST - SW writes 0 to release of reset
#define CONST_TSO_GCTRL_TOE_RST_TOENRST 0x0
//! Constant TOERST - SW writes 1 to reset
#define CONST_TSO_GCTRL_TOE_RST_TOERST 0x1

//! Field OCP_CTRL_ENDI_B - Control OCP SWAPPER byte
#define TSO_GCTRL_OCP_CTRL_ENDI_B_POS 18
//! Field OCP_CTRL_ENDI_B - Control OCP SWAPPER byte
#define TSO_GCTRL_OCP_CTRL_ENDI_B_MASK 0x40000u
//! Constant NO_SWP
#define CONST_TSO_GCTRL_OCP_CTRL_ENDI_B_NO_SWP 0x0
//! Constant SWAP
#define CONST_TSO_GCTRL_OCP_CTRL_ENDI_B_SWAP 0x1

//! Field OCP_CTRL_ENDI_W - Control OCP SWAPPER WORD
#define TSO_GCTRL_OCP_CTRL_ENDI_W_POS 19
//! Field OCP_CTRL_ENDI_W - Control OCP SWAPPER WORD
#define TSO_GCTRL_OCP_CTRL_ENDI_W_MASK 0x80000u
//! Constant NO_SWP
#define CONST_TSO_GCTRL_OCP_CTRL_ENDI_W_NO_SWP 0x0
//! Constant SWAP
#define CONST_TSO_GCTRL_OCP_CTRL_ENDI_W_SWAP 0x1

//! Field LRO_MAX_AGP - LRO Maximum numbers of Packets to be aggregated
#define TSO_GCTRL_LRO_MAX_AGP_POS 24
//! Field LRO_MAX_AGP - LRO Maximum numbers of Packets to be aggregated
#define TSO_GCTRL_LRO_MAX_AGP_MASK 0x1F000000u

//! Field DEBUG_PAUSE_TSO - PAUSE HW for Debug Access of TSO
#define TSO_GCTRL_DEBUG_PAUSE_TSO_POS 30
//! Field DEBUG_PAUSE_TSO - PAUSE HW for Debug Access of TSO
#define TSO_GCTRL_DEBUG_PAUSE_TSO_MASK 0x40000000u
//! Constant NORM - Normal operating
#define CONST_TSO_GCTRL_DEBUG_PAUSE_TSO_NORM 0x0
//! Constant PAUSE - force HW pause
#define CONST_TSO_GCTRL_DEBUG_PAUSE_TSO_PAUSE 0x1

//! Field DEBUG_PAUSE_LRO - PAUSE HW for Debug Access of LRO
#define TSO_GCTRL_DEBUG_PAUSE_LRO_POS 31
//! Field DEBUG_PAUSE_LRO - PAUSE HW for Debug Access of LRO
#define TSO_GCTRL_DEBUG_PAUSE_LRO_MASK 0x80000000u
//! Constant NORM - Normal operating
#define CONST_TSO_GCTRL_DEBUG_PAUSE_LRO_NORM 0x0
//! Constant PAUSE - force HW pause
#define CONST_TSO_GCTRL_DEBUG_PAUSE_LRO_PAUSE 0x1

//! @}

//! \defgroup POLL_INTERVAL Register POLL_INTERVAL - Global polling interval counter for TOE HW polling events
//! @{

//! Register Offset (relative)
#define POLL_INTERVAL 0x204

//! Register Reset Value
#define POLL_INTERVAL_RST 0x00000000u

//! Field POLL - set the polling interval
#define POLL_INTERVAL_POLL_POS 0
//! Field POLL - set the polling interval
#define POLL_INTERVAL_POLL_MASK 0xFFFFFFFFu

//! @}

//! \defgroup HDR_BASE_SEG0 Register HDR_BASE_SEG0 - Header Base Address for Segment
//! @{

//! Register Offset (relative)
#define HDR_BASE_SEG0 0x220

//! Register Reset Value
#define HDR_BASE_SEG0_RST 0x00000000u

//! Field BASE - Header Base Address for Segment
#define HDR_BASE_SEG0_BASE_POS 0
//! Field BASE - Header Base Address for Segment
#define HDR_BASE_SEG0_BASE_MASK 0xFFFFFFFFu

//! @}

//! \defgroup HDR_BASE_SEG1 Register HDR_BASE_SEG1 - Header Base Address for Segment
//! @{

//! Register Offset (relative)
#define HDR_BASE_SEG1 0x224

//! Register Reset Value
#define HDR_BASE_SEG1_RST 0x00000000u

//! Field BASE - Header Base Address for Segment
#define HDR_BASE_SEG1_BASE_POS 0
//! Field BASE - Header Base Address for Segment
#define HDR_BASE_SEG1_BASE_MASK 0xFFFFFFFFu

//! @}

//! \defgroup MIPS_CFG_0 Register MIPS_CFG_0 - EVA Mapping Info Register
//! @{

//! Register Offset (relative)
#define MIPS_CFG_0 0x240

//! Register Reset Value
#define MIPS_CFG_0_RST 0x00000000u

//! Field CFG0 - EVA Mapping information for MIPS SegCtl[2:0] split into 6 registers
#define MIPS_CFG_0_CFG0_POS 0
//! Field CFG0 - EVA Mapping information for MIPS SegCtl[2:0] split into 6 registers
#define MIPS_CFG_0_CFG0_MASK 0xFFFFu

//! @}

//! \defgroup MIPS_CFG_1 Register MIPS_CFG_1 - EVA Mapping Info Register
//! @{

//! Register Offset (relative)
#define MIPS_CFG_1 0x250

//! Register Reset Value
#define MIPS_CFG_1_RST 0x00000000u

//! Field CFG0 - EVA Mapping information for MIPS SegCtl[2:0] split into 6 registers
#define MIPS_CFG_1_CFG0_POS 0
//! Field CFG0 - EVA Mapping information for MIPS SegCtl[2:0] split into 6 registers
#define MIPS_CFG_1_CFG0_MASK 0xFFFFu

//! @}

//! \defgroup MIPS_CFG_2 Register MIPS_CFG_2 - EVA Mapping Info Register
//! @{

//! Register Offset (relative)
#define MIPS_CFG_2 0x260

//! Register Reset Value
#define MIPS_CFG_2_RST 0x00000000u

//! Field CFG0 - EVA Mapping information for MIPS SegCtl[2:0] split into 6 registers
#define MIPS_CFG_2_CFG0_POS 0
//! Field CFG0 - EVA Mapping information for MIPS SegCtl[2:0] split into 6 registers
#define MIPS_CFG_2_CFG0_MASK 0xFFFFu

//! @}

//! \defgroup MIPS_CFG_3 Register MIPS_CFG_3 - EVA Mapping Info Register
//! @{

//! Register Offset (relative)
#define MIPS_CFG_3 0x270

//! Register Reset Value
#define MIPS_CFG_3_RST 0x00000000u

//! Field CFG0 - EVA Mapping information for MIPS SegCtl[2:0] split into 6 registers
#define MIPS_CFG_3_CFG0_POS 0
//! Field CFG0 - EVA Mapping information for MIPS SegCtl[2:0] split into 6 registers
#define MIPS_CFG_3_CFG0_MASK 0xFFFFu

//! @}

//! \defgroup MIPS_CFG_4 Register MIPS_CFG_4 - EVA Mapping Info Register
//! @{

//! Register Offset (relative)
#define MIPS_CFG_4 0x280

//! Register Reset Value
#define MIPS_CFG_4_RST 0x00000000u

//! Field CFG0 - EVA Mapping information for MIPS SegCtl[2:0] split into 6 registers
#define MIPS_CFG_4_CFG0_POS 0
//! Field CFG0 - EVA Mapping information for MIPS SegCtl[2:0] split into 6 registers
#define MIPS_CFG_4_CFG0_MASK 0xFFFFu

//! @}

//! \defgroup MIPS_CFG_5 Register MIPS_CFG_5 - EVA Mapping Info Register
//! @{

//! Register Offset (relative)
#define MIPS_CFG_5 0x290

//! Register Reset Value
#define MIPS_CFG_5_RST 0x00000000u

//! Field CFG0 - EVA Mapping information for MIPS SegCtl[2:0] split into 6 registers
#define MIPS_CFG_5_CFG0_POS 0
//! Field CFG0 - EVA Mapping information for MIPS SegCtl[2:0] split into 6 registers
#define MIPS_CFG_5_CFG0_MASK 0xFFFFu

//! @}

//! \defgroup LRO_FID_0 Register LRO_FID_0 - FLOW ID for LRO Session
//! @{

//! Register Offset (relative)
#define LRO_FID_0 0x1000

//! Register Reset Value
#define LRO_FID_0_RST 0xC0000000u

//! Field LRO_FID - FLOW ID for LRO Session
#define LRO_FID_0_LRO_FID_POS 0
//! Field LRO_FID - FLOW ID for LRO Session
#define LRO_FID_0_LRO_FID_MASK 0x3Fu

//! Field S_END - SW Indicates that session is ended
#define LRO_FID_0_S_END_POS 30
//! Field S_END - SW Indicates that session is ended
#define LRO_FID_0_S_END_MASK 0x40000000u
//! Constant NEND - Not end
#define CONST_LRO_FID_0_S_END_NEND 0x0
//! Constant END - Session end
#define CONST_LRO_FID_0_S_END_END 0x1

//! Field OWNER - OWNER bit of the Command
#define LRO_FID_0_OWNER_POS 31
//! Field OWNER - OWNER bit of the Command
#define LRO_FID_0_OWNER_MASK 0x80000000u
//! Constant HW - HW owns it
#define CONST_LRO_FID_0_OWNER_HW 0x0
//! Constant CPU - CPU owns it
#define CONST_LRO_FID_0_OWNER_CPU 0x1

//! @}

//! \defgroup LRO_TO_REG_0 Register LRO_TO_REG_0 - LRO Time Out Window
//! @{

//! Register Offset (relative)
#define LRO_TO_REG_0 0x1004

//! Register Reset Value
#define LRO_TO_REG_0_RST 0x00000000u

//! Field LRO_TO - LRO Time Out Window
#define LRO_TO_REG_0_LRO_TO_POS 0
//! Field LRO_TO - LRO Time Out Window
#define LRO_TO_REG_0_LRO_TO_MASK 0xFFFFFFFFu

//! @}

//! \defgroup LRO_FID_1 Register LRO_FID_1 - FLOW ID for LRO Session
//! @{

//! Register Offset (relative)
#define LRO_FID_1 0x1008

//! Register Reset Value
#define LRO_FID_1_RST 0xC0000000u

//! Field LRO_FID - FLOW ID for LRO Session
#define LRO_FID_1_LRO_FID_POS 0
//! Field LRO_FID - FLOW ID for LRO Session
#define LRO_FID_1_LRO_FID_MASK 0x3Fu

//! Field S_END - SW Indicates that session is ended
#define LRO_FID_1_S_END_POS 30
//! Field S_END - SW Indicates that session is ended
#define LRO_FID_1_S_END_MASK 0x40000000u
//! Constant NEND - Not end
#define CONST_LRO_FID_1_S_END_NEND 0x0
//! Constant END - Session end
#define CONST_LRO_FID_1_S_END_END 0x1

//! Field OWNER - OWNER bit of the Command
#define LRO_FID_1_OWNER_POS 31
//! Field OWNER - OWNER bit of the Command
#define LRO_FID_1_OWNER_MASK 0x80000000u
//! Constant HW - HW owns it
#define CONST_LRO_FID_1_OWNER_HW 0x0
//! Constant CPU - CPU owns it
#define CONST_LRO_FID_1_OWNER_CPU 0x1

//! @}

//! \defgroup LRO_TO_REG_1 Register LRO_TO_REG_1 - LRO Time Out Window
//! @{

//! Register Offset (relative)
#define LRO_TO_REG_1 0x100C

//! Register Reset Value
#define LRO_TO_REG_1_RST 0x00000000u

//! Field LRO_TO - LRO Time Out Window
#define LRO_TO_REG_1_LRO_TO_POS 0
//! Field LRO_TO - LRO Time Out Window
#define LRO_TO_REG_1_LRO_TO_MASK 0xFFFFFFFFu

//! @}

//! \defgroup LRO_FID_2 Register LRO_FID_2 - FLOW ID for LRO Session
//! @{

//! Register Offset (relative)
#define LRO_FID_2 0x1010

//! Register Reset Value
#define LRO_FID_2_RST 0xC0000000u

//! Field LRO_FID - FLOW ID for LRO Session
#define LRO_FID_2_LRO_FID_POS 0
//! Field LRO_FID - FLOW ID for LRO Session
#define LRO_FID_2_LRO_FID_MASK 0x3Fu

//! Field S_END - SW Indicates that session is ended
#define LRO_FID_2_S_END_POS 30
//! Field S_END - SW Indicates that session is ended
#define LRO_FID_2_S_END_MASK 0x40000000u
//! Constant NEND - Not end
#define CONST_LRO_FID_2_S_END_NEND 0x0
//! Constant END - Session end
#define CONST_LRO_FID_2_S_END_END 0x1

//! Field OWNER - OWNER bit of the Command
#define LRO_FID_2_OWNER_POS 31
//! Field OWNER - OWNER bit of the Command
#define LRO_FID_2_OWNER_MASK 0x80000000u
//! Constant HW - HW owns it
#define CONST_LRO_FID_2_OWNER_HW 0x0
//! Constant CPU - CPU owns it
#define CONST_LRO_FID_2_OWNER_CPU 0x1

//! @}

//! \defgroup LRO_TO_REG_2 Register LRO_TO_REG_2 - LRO Time Out Window
//! @{

//! Register Offset (relative)
#define LRO_TO_REG_2 0x1014

//! Register Reset Value
#define LRO_TO_REG_2_RST 0x00000000u

//! Field LRO_TO - LRO Time Out Window
#define LRO_TO_REG_2_LRO_TO_POS 0
//! Field LRO_TO - LRO Time Out Window
#define LRO_TO_REG_2_LRO_TO_MASK 0xFFFFFFFFu

//! @}

//! \defgroup LRO_FID_3 Register LRO_FID_3 - FLOW ID for LRO Session
//! @{

//! Register Offset (relative)
#define LRO_FID_3 0x1018

//! Register Reset Value
#define LRO_FID_3_RST 0xC0000000u

//! Field LRO_FID - FLOW ID for LRO Session
#define LRO_FID_3_LRO_FID_POS 0
//! Field LRO_FID - FLOW ID for LRO Session
#define LRO_FID_3_LRO_FID_MASK 0x3Fu

//! Field S_END - SW Indicates that session is ended
#define LRO_FID_3_S_END_POS 30
//! Field S_END - SW Indicates that session is ended
#define LRO_FID_3_S_END_MASK 0x40000000u
//! Constant NEND - Not end
#define CONST_LRO_FID_3_S_END_NEND 0x0
//! Constant END - Session end
#define CONST_LRO_FID_3_S_END_END 0x1

//! Field OWNER - OWNER bit of the Command
#define LRO_FID_3_OWNER_POS 31
//! Field OWNER - OWNER bit of the Command
#define LRO_FID_3_OWNER_MASK 0x80000000u
//! Constant HW - HW owns it
#define CONST_LRO_FID_3_OWNER_HW 0x0
//! Constant CPU - CPU owns it
#define CONST_LRO_FID_3_OWNER_CPU 0x1

//! @}

//! \defgroup LRO_TO_REG_3 Register LRO_TO_REG_3 - LRO Time Out Window
//! @{

//! Register Offset (relative)
#define LRO_TO_REG_3 0x101C

//! Register Reset Value
#define LRO_TO_REG_3_RST 0x00000000u

//! Field LRO_TO - LRO Time Out Window
#define LRO_TO_REG_3_LRO_TO_POS 0
//! Field LRO_TO - LRO Time Out Window
#define LRO_TO_REG_3_LRO_TO_MASK 0xFFFFFFFFu

//! @}

//! \defgroup LRO_FID_4 Register LRO_FID_4 - FLOW ID for LRO Session
//! @{

//! Register Offset (relative)
#define LRO_FID_4 0x1020

//! Register Reset Value
#define LRO_FID_4_RST 0xC0000000u

//! Field LRO_FID - FLOW ID for LRO Session
#define LRO_FID_4_LRO_FID_POS 0
//! Field LRO_FID - FLOW ID for LRO Session
#define LRO_FID_4_LRO_FID_MASK 0x3Fu

//! Field S_END - SW Indicates that session is ended
#define LRO_FID_4_S_END_POS 30
//! Field S_END - SW Indicates that session is ended
#define LRO_FID_4_S_END_MASK 0x40000000u
//! Constant NEND - Not end
#define CONST_LRO_FID_4_S_END_NEND 0x0
//! Constant END - Session end
#define CONST_LRO_FID_4_S_END_END 0x1

//! Field OWNER - OWNER bit of the Command
#define LRO_FID_4_OWNER_POS 31
//! Field OWNER - OWNER bit of the Command
#define LRO_FID_4_OWNER_MASK 0x80000000u
//! Constant HW - HW owns it
#define CONST_LRO_FID_4_OWNER_HW 0x0
//! Constant CPU - CPU owns it
#define CONST_LRO_FID_4_OWNER_CPU 0x1

//! @}

//! \defgroup LRO_TO_REG_4 Register LRO_TO_REG_4 - LRO Time Out Window
//! @{

//! Register Offset (relative)
#define LRO_TO_REG_4 0x1024

//! Register Reset Value
#define LRO_TO_REG_4_RST 0x00000000u

//! Field LRO_TO - LRO Time Out Window
#define LRO_TO_REG_4_LRO_TO_POS 0
//! Field LRO_TO - LRO Time Out Window
#define LRO_TO_REG_4_LRO_TO_MASK 0xFFFFFFFFu

//! @}

//! \defgroup LRO_FID_5 Register LRO_FID_5 - FLOW ID for LRO Session
//! @{

//! Register Offset (relative)
#define LRO_FID_5 0x1028

//! Register Reset Value
#define LRO_FID_5_RST 0xC0000000u

//! Field LRO_FID - FLOW ID for LRO Session
#define LRO_FID_5_LRO_FID_POS 0
//! Field LRO_FID - FLOW ID for LRO Session
#define LRO_FID_5_LRO_FID_MASK 0x3Fu

//! Field S_END - SW Indicates that session is ended
#define LRO_FID_5_S_END_POS 30
//! Field S_END - SW Indicates that session is ended
#define LRO_FID_5_S_END_MASK 0x40000000u
//! Constant NEND - Not end
#define CONST_LRO_FID_5_S_END_NEND 0x0
//! Constant END - Session end
#define CONST_LRO_FID_5_S_END_END 0x1

//! Field OWNER - OWNER bit of the Command
#define LRO_FID_5_OWNER_POS 31
//! Field OWNER - OWNER bit of the Command
#define LRO_FID_5_OWNER_MASK 0x80000000u
//! Constant HW - HW owns it
#define CONST_LRO_FID_5_OWNER_HW 0x0
//! Constant CPU - CPU owns it
#define CONST_LRO_FID_5_OWNER_CPU 0x1

//! @}

//! \defgroup LRO_TO_REG_5 Register LRO_TO_REG_5 - LRO Time Out Window
//! @{

//! Register Offset (relative)
#define LRO_TO_REG_5 0x102C

//! Register Reset Value
#define LRO_TO_REG_5_RST 0x00000000u

//! Field LRO_TO - LRO Time Out Window
#define LRO_TO_REG_5_LRO_TO_POS 0
//! Field LRO_TO - LRO Time Out Window
#define LRO_TO_REG_5_LRO_TO_MASK 0xFFFFFFFFu

//! @}

//! \defgroup LRO_FID_6 Register LRO_FID_6 - FLOW ID for LRO Session
//! @{

//! Register Offset (relative)
#define LRO_FID_6 0x1030

//! Register Reset Value
#define LRO_FID_6_RST 0xC0000000u

//! Field LRO_FID - FLOW ID for LRO Session
#define LRO_FID_6_LRO_FID_POS 0
//! Field LRO_FID - FLOW ID for LRO Session
#define LRO_FID_6_LRO_FID_MASK 0x3Fu

//! Field S_END - SW Indicates that session is ended
#define LRO_FID_6_S_END_POS 30
//! Field S_END - SW Indicates that session is ended
#define LRO_FID_6_S_END_MASK 0x40000000u
//! Constant NEND - Not end
#define CONST_LRO_FID_6_S_END_NEND 0x0
//! Constant END - Session end
#define CONST_LRO_FID_6_S_END_END 0x1

//! Field OWNER - OWNER bit of the Command
#define LRO_FID_6_OWNER_POS 31
//! Field OWNER - OWNER bit of the Command
#define LRO_FID_6_OWNER_MASK 0x80000000u
//! Constant HW - HW owns it
#define CONST_LRO_FID_6_OWNER_HW 0x0
//! Constant CPU - CPU owns it
#define CONST_LRO_FID_6_OWNER_CPU 0x1

//! @}

//! \defgroup LRO_TO_REG_6 Register LRO_TO_REG_6 - LRO Time Out Window
//! @{

//! Register Offset (relative)
#define LRO_TO_REG_6 0x1034

//! Register Reset Value
#define LRO_TO_REG_6_RST 0x00000000u

//! Field LRO_TO - LRO Time Out Window
#define LRO_TO_REG_6_LRO_TO_POS 0
//! Field LRO_TO - LRO Time Out Window
#define LRO_TO_REG_6_LRO_TO_MASK 0xFFFFFFFFu

//! @}

//! \defgroup LRO_FID_7 Register LRO_FID_7 - FLOW ID for LRO Session
//! @{

//! Register Offset (relative)
#define LRO_FID_7 0x1038

//! Register Reset Value
#define LRO_FID_7_RST 0xC0000000u

//! Field LRO_FID - FLOW ID for LRO Session
#define LRO_FID_7_LRO_FID_POS 0
//! Field LRO_FID - FLOW ID for LRO Session
#define LRO_FID_7_LRO_FID_MASK 0x3Fu

//! Field S_END - SW Indicates that session is ended
#define LRO_FID_7_S_END_POS 30
//! Field S_END - SW Indicates that session is ended
#define LRO_FID_7_S_END_MASK 0x40000000u
//! Constant NEND - Not end
#define CONST_LRO_FID_7_S_END_NEND 0x0
//! Constant END - Session end
#define CONST_LRO_FID_7_S_END_END 0x1

//! Field OWNER - OWNER bit of the Command
#define LRO_FID_7_OWNER_POS 31
//! Field OWNER - OWNER bit of the Command
#define LRO_FID_7_OWNER_MASK 0x80000000u
//! Constant HW - HW owns it
#define CONST_LRO_FID_7_OWNER_HW 0x0
//! Constant CPU - CPU owns it
#define CONST_LRO_FID_7_OWNER_CPU 0x1

//! @}

//! \defgroup LRO_TO_REG_7 Register LRO_TO_REG_7 - LRO Time Out Window
//! @{

//! Register Offset (relative)
#define LRO_TO_REG_7 0x103C

//! Register Reset Value
#define LRO_TO_REG_7_RST 0x00000000u

//! Field LRO_TO - LRO Time Out Window
#define LRO_TO_REG_7_LRO_TO_POS 0
//! Field LRO_TO - LRO Time Out Window
#define LRO_TO_REG_7_LRO_TO_MASK 0xFFFFFFFFu

//! @}

//! \defgroup TOE_INT_MASK Register TOE_INT_MASK - Module Interrupt Mask register
//! @{

//! Register Offset (relative)
#define TOE_INT_MASK 0x1044

//! Register Reset Value
#define TOE_INT_MASK_RST 0x00000000u

//! Field TOE0 - Interrupt Mask bit
#define TOE_INT_MASK_TOE0_POS 0
//! Field TOE0 - Interrupt Mask bit
#define TOE_INT_MASK_TOE0_MASK 0x1u
//! Constant UM - Mask out this bit
#define CONST_TOE_INT_MASK_TOE0_UM 0x0
//! Constant M - Mask to Select this bit
#define CONST_TOE_INT_MASK_TOE0_M 0x1

//! Field TOE1 - Interrupt Mask bit
#define TOE_INT_MASK_TOE1_POS 1
//! Field TOE1 - Interrupt Mask bit
#define TOE_INT_MASK_TOE1_MASK 0x2u
//! Constant UM - Mask out this bit
#define CONST_TOE_INT_MASK_TOE1_UM 0x0
//! Constant M - Mask to Select this bit
#define CONST_TOE_INT_MASK_TOE1_M 0x1

//! Field TOE2 - Interrupt Mask bit
#define TOE_INT_MASK_TOE2_POS 2
//! Field TOE2 - Interrupt Mask bit
#define TOE_INT_MASK_TOE2_MASK 0x4u
//! Constant UM - Mask out this bit
#define CONST_TOE_INT_MASK_TOE2_UM 0x0
//! Constant M - Mask to Select this bit
#define CONST_TOE_INT_MASK_TOE2_M 0x1

//! Field TOE3 - Interrupt Mask bit
#define TOE_INT_MASK_TOE3_POS 3
//! Field TOE3 - Interrupt Mask bit
#define TOE_INT_MASK_TOE3_MASK 0x8u
//! Constant UM - Mask out this bit
#define CONST_TOE_INT_MASK_TOE3_UM 0x0
//! Constant M - Mask to Select this bit
#define CONST_TOE_INT_MASK_TOE3_M 0x1

//! Field LRO0 - Interrupt Mask bit
#define TOE_INT_MASK_LRO0_POS 4
//! Field LRO0 - Interrupt Mask bit
#define TOE_INT_MASK_LRO0_MASK 0x10u
//! Constant UM - Mask out this bit
#define CONST_TOE_INT_MASK_LRO0_UM 0x0
//! Constant M - Mask to Select this bit
#define CONST_TOE_INT_MASK_LRO0_M 0x1

//! Field LRO1 - Interrupt Mask bit
#define TOE_INT_MASK_LRO1_POS 5
//! Field LRO1 - Interrupt Mask bit
#define TOE_INT_MASK_LRO1_MASK 0x20u
//! Constant UM - Mask out this bit
#define CONST_TOE_INT_MASK_LRO1_UM 0x0
//! Constant M - Mask to Select this bit
#define CONST_TOE_INT_MASK_LRO1_M 0x1

//! Field LRO2 - Interrupt Mask bit
#define TOE_INT_MASK_LRO2_POS 6
//! Field LRO2 - Interrupt Mask bit
#define TOE_INT_MASK_LRO2_MASK 0x40u
//! Constant UM - Mask out this bit
#define CONST_TOE_INT_MASK_LRO2_UM 0x0
//! Constant M - Mask to Select this bit
#define CONST_TOE_INT_MASK_LRO2_M 0x1

//! Field LRO3 - Interrupt Mask bit
#define TOE_INT_MASK_LRO3_POS 7
//! Field LRO3 - Interrupt Mask bit
#define TOE_INT_MASK_LRO3_MASK 0x80u
//! Constant UM - Mask out this bit
#define CONST_TOE_INT_MASK_LRO3_UM 0x0
//! Constant M - Mask to Select this bit
#define CONST_TOE_INT_MASK_LRO3_M 0x1

//! Field LRO4 - Interrupt Mask bit
#define TOE_INT_MASK_LRO4_POS 8
//! Field LRO4 - Interrupt Mask bit
#define TOE_INT_MASK_LRO4_MASK 0x100u
//! Constant UM - Mask out this bit
#define CONST_TOE_INT_MASK_LRO4_UM 0x0
//! Constant M - Mask to Select this bit
#define CONST_TOE_INT_MASK_LRO4_M 0x1

//! Field LRO5 - Interrupt Mask bit
#define TOE_INT_MASK_LRO5_POS 9
//! Field LRO5 - Interrupt Mask bit
#define TOE_INT_MASK_LRO5_MASK 0x200u
//! Constant UM - Mask out this bit
#define CONST_TOE_INT_MASK_LRO5_UM 0x0
//! Constant M - Mask to Select this bit
#define CONST_TOE_INT_MASK_LRO5_M 0x1

//! Field LRO6 - Interrupt Mask bit
#define TOE_INT_MASK_LRO6_POS 10
//! Field LRO6 - Interrupt Mask bit
#define TOE_INT_MASK_LRO6_MASK 0x400u
//! Constant UM - Mask out this bit
#define CONST_TOE_INT_MASK_LRO6_UM 0x0
//! Constant M - Mask to Select this bit
#define CONST_TOE_INT_MASK_LRO6_M 0x1

//! Field LRO7 - Interrupt Mask bit
#define TOE_INT_MASK_LRO7_POS 11
//! Field LRO7 - Interrupt Mask bit
#define TOE_INT_MASK_LRO7_MASK 0x800u
//! Constant UM - Mask out this bit
#define CONST_TOE_INT_MASK_LRO7_UM 0x0
//! Constant M - Mask to Select this bit
#define CONST_TOE_INT_MASK_LRO7_M 0x1

//! Field LRO_EXP - Interrupt Mask bit
#define TOE_INT_MASK_LRO_EXP_POS 12
//! Field LRO_EXP - Interrupt Mask bit
#define TOE_INT_MASK_LRO_EXP_MASK 0x1000u
//! Constant UM - Mask out this bit
#define CONST_TOE_INT_MASK_LRO_EXP_UM 0x0
//! Constant M - Mask to Select this bit
#define CONST_TOE_INT_MASK_LRO_EXP_M 0x1

//! Field S16 - Interrupt Mask bit
#define TOE_INT_MASK_S16_POS 16
//! Field S16 - Interrupt Mask bit
#define TOE_INT_MASK_S16_MASK 0x10000u
//! Constant UM - Mask out this bit
#define CONST_TOE_INT_MASK_S16_UM 0x0
//! Constant M - Mask to Select this bit
#define CONST_TOE_INT_MASK_S16_M 0x1

//! Field S17 - Interrupt Mask bit
#define TOE_INT_MASK_S17_POS 17
//! Field S17 - Interrupt Mask bit
#define TOE_INT_MASK_S17_MASK 0x20000u
//! Constant UM - Mask out this bit
#define CONST_TOE_INT_MASK_S17_UM 0x0
//! Constant M - Mask to Select this bit
#define CONST_TOE_INT_MASK_S17_M 0x1

//! Field S18 - Interrupt Mask bit
#define TOE_INT_MASK_S18_POS 18
//! Field S18 - Interrupt Mask bit
#define TOE_INT_MASK_S18_MASK 0x40000u
//! Constant UM - Mask out this bit
#define CONST_TOE_INT_MASK_S18_UM 0x0
//! Constant M - Mask to Select this bit
#define CONST_TOE_INT_MASK_S18_M 0x1

//! Field S19 - Interrupt Mask bit
#define TOE_INT_MASK_S19_POS 19
//! Field S19 - Interrupt Mask bit
#define TOE_INT_MASK_S19_MASK 0x80000u
//! Constant UM - Mask out this bit
#define CONST_TOE_INT_MASK_S19_UM 0x0
//! Constant M - Mask to Select this bit
#define CONST_TOE_INT_MASK_S19_M 0x1

//! Field S20 - Interrupt Mask bit
#define TOE_INT_MASK_S20_POS 20
//! Field S20 - Interrupt Mask bit
#define TOE_INT_MASK_S20_MASK 0x100000u
//! Constant UM - Mask out this bit
#define CONST_TOE_INT_MASK_S20_UM 0x0
//! Constant M - Mask to Select this bit
#define CONST_TOE_INT_MASK_S20_M 0x1

//! Field S21 - Interrupt Mask bit
#define TOE_INT_MASK_S21_POS 21
//! Field S21 - Interrupt Mask bit
#define TOE_INT_MASK_S21_MASK 0x200000u
//! Constant UM - Mask out this bit
#define CONST_TOE_INT_MASK_S21_UM 0x0
//! Constant M - Mask to Select this bit
#define CONST_TOE_INT_MASK_S21_M 0x1

//! Field S22 - Interrupt Mask bit
#define TOE_INT_MASK_S22_POS 22
//! Field S22 - Interrupt Mask bit
#define TOE_INT_MASK_S22_MASK 0x400000u
//! Constant UM - Mask out this bit
#define CONST_TOE_INT_MASK_S22_UM 0x0
//! Constant M - Mask to Select this bit
#define CONST_TOE_INT_MASK_S22_M 0x1

//! Field S23 - Interrupt Mask bit
#define TOE_INT_MASK_S23_POS 23
//! Field S23 - Interrupt Mask bit
#define TOE_INT_MASK_S23_MASK 0x800000u
//! Constant UM - Mask out this bit
#define CONST_TOE_INT_MASK_S23_UM 0x0
//! Constant M - Mask to Select this bit
#define CONST_TOE_INT_MASK_S23_M 0x1

//! @}

//! \defgroup TOE_INT_EN Register TOE_INT_EN - Module Interrupt EN register
//! @{

//! Register Offset (relative)
#define TOE_INT_EN 0x1048

//! Register Reset Value
#define TOE_INT_EN_RST 0x00000000u

//! Field TOE0 - Interrupt Enable bit
#define TOE_INT_EN_TOE0_POS 0
//! Field TOE0 - Interrupt Enable bit
#define TOE_INT_EN_TOE0_MASK 0x1u
//! Constant UEN - Disable this bit
#define CONST_TOE_INT_EN_TOE0_UEN 0x0
//! Constant EN - Enable this bit
#define CONST_TOE_INT_EN_TOE0_EN 0x1

//! Field TOE1 - Interrupt Enable bit
#define TOE_INT_EN_TOE1_POS 1
//! Field TOE1 - Interrupt Enable bit
#define TOE_INT_EN_TOE1_MASK 0x2u
//! Constant UEN - Disable this bit
#define CONST_TOE_INT_EN_TOE1_UEN 0x0
//! Constant EN - Enable this bit
#define CONST_TOE_INT_EN_TOE1_EN 0x1

//! Field TOE2 - Interrupt Enable bit
#define TOE_INT_EN_TOE2_POS 2
//! Field TOE2 - Interrupt Enable bit
#define TOE_INT_EN_TOE2_MASK 0x4u
//! Constant UEN - Disable this bit
#define CONST_TOE_INT_EN_TOE2_UEN 0x0
//! Constant EN - Enable this bit
#define CONST_TOE_INT_EN_TOE2_EN 0x1

//! Field TOE3 - Interrupt Enable bit
#define TOE_INT_EN_TOE3_POS 3
//! Field TOE3 - Interrupt Enable bit
#define TOE_INT_EN_TOE3_MASK 0x8u
//! Constant UEN - Disable this bit
#define CONST_TOE_INT_EN_TOE3_UEN 0x0
//! Constant EN - Enable this bit
#define CONST_TOE_INT_EN_TOE3_EN 0x1

//! Field LRO0 - Interrupt Enable bit
#define TOE_INT_EN_LRO0_POS 4
//! Field LRO0 - Interrupt Enable bit
#define TOE_INT_EN_LRO0_MASK 0x10u
//! Constant UEN - Disable this bit
#define CONST_TOE_INT_EN_LRO0_UEN 0x0
//! Constant EN - Enable this bit
#define CONST_TOE_INT_EN_LRO0_EN 0x1

//! Field LRO1 - Interrupt Enable bit
#define TOE_INT_EN_LRO1_POS 5
//! Field LRO1 - Interrupt Enable bit
#define TOE_INT_EN_LRO1_MASK 0x20u
//! Constant UEN - Disable this bit
#define CONST_TOE_INT_EN_LRO1_UEN 0x0
//! Constant EN - Enable this bit
#define CONST_TOE_INT_EN_LRO1_EN 0x1

//! Field LRO2 - Interrupt Enable bit
#define TOE_INT_EN_LRO2_POS 6
//! Field LRO2 - Interrupt Enable bit
#define TOE_INT_EN_LRO2_MASK 0x40u
//! Constant UEN - Disable this bit
#define CONST_TOE_INT_EN_LRO2_UEN 0x0
//! Constant EN - Enable this bit
#define CONST_TOE_INT_EN_LRO2_EN 0x1

//! Field LRO3 - Interrupt Enable bit
#define TOE_INT_EN_LRO3_POS 7
//! Field LRO3 - Interrupt Enable bit
#define TOE_INT_EN_LRO3_MASK 0x80u
//! Constant UEN - Disable this bit
#define CONST_TOE_INT_EN_LRO3_UEN 0x0
//! Constant EN - Enable this bit
#define CONST_TOE_INT_EN_LRO3_EN 0x1

//! Field LRO4 - Interrupt Enable bit
#define TOE_INT_EN_LRO4_POS 8
//! Field LRO4 - Interrupt Enable bit
#define TOE_INT_EN_LRO4_MASK 0x100u
//! Constant UEN - Disable this bit
#define CONST_TOE_INT_EN_LRO4_UEN 0x0
//! Constant EN - Enable this bit
#define CONST_TOE_INT_EN_LRO4_EN 0x1

//! Field LRO5 - Interrupt Enable bit
#define TOE_INT_EN_LRO5_POS 9
//! Field LRO5 - Interrupt Enable bit
#define TOE_INT_EN_LRO5_MASK 0x200u
//! Constant UEN - Disable this bit
#define CONST_TOE_INT_EN_LRO5_UEN 0x0
//! Constant EN - Enable this bit
#define CONST_TOE_INT_EN_LRO5_EN 0x1

//! Field LRO6 - Interrupt Enable bit
#define TOE_INT_EN_LRO6_POS 10
//! Field LRO6 - Interrupt Enable bit
#define TOE_INT_EN_LRO6_MASK 0x400u
//! Constant UEN - Disable this bit
#define CONST_TOE_INT_EN_LRO6_UEN 0x0
//! Constant EN - Enable this bit
#define CONST_TOE_INT_EN_LRO6_EN 0x1

//! Field LRO7 - Interrupt Enable bit
#define TOE_INT_EN_LRO7_POS 11
//! Field LRO7 - Interrupt Enable bit
#define TOE_INT_EN_LRO7_MASK 0x800u
//! Constant UEN - Disable this bit
#define CONST_TOE_INT_EN_LRO7_UEN 0x0
//! Constant EN - Enable this bit
#define CONST_TOE_INT_EN_LRO7_EN 0x1

//! Field LRO_EXP - Interrupt Enable bit
#define TOE_INT_EN_LRO_EXP_POS 12
//! Field LRO_EXP - Interrupt Enable bit
#define TOE_INT_EN_LRO_EXP_MASK 0x1000u
//! Constant UEN - Disable this bit
#define CONST_TOE_INT_EN_LRO_EXP_UEN 0x0
//! Constant EN - Enable this bit
#define CONST_TOE_INT_EN_LRO_EXP_EN 0x1

//! Field S16 - Interrupt Enable bit
#define TOE_INT_EN_S16_POS 16
//! Field S16 - Interrupt Enable bit
#define TOE_INT_EN_S16_MASK 0x10000u
//! Constant UEN - Disable this bit
#define CONST_TOE_INT_EN_S16_UEN 0x0
//! Constant EN - Enable this bit
#define CONST_TOE_INT_EN_S16_EN 0x1

//! Field S17 - Interrupt Enable bit
#define TOE_INT_EN_S17_POS 17
//! Field S17 - Interrupt Enable bit
#define TOE_INT_EN_S17_MASK 0x20000u
//! Constant UEN - Disable this bit
#define CONST_TOE_INT_EN_S17_UEN 0x0
//! Constant EN - Enable this bit
#define CONST_TOE_INT_EN_S17_EN 0x1

//! Field S18 - Interrupt Enable bit
#define TOE_INT_EN_S18_POS 18
//! Field S18 - Interrupt Enable bit
#define TOE_INT_EN_S18_MASK 0x40000u
//! Constant UEN - Disable this bit
#define CONST_TOE_INT_EN_S18_UEN 0x0
//! Constant EN - Enable this bit
#define CONST_TOE_INT_EN_S18_EN 0x1

//! Field S19 - Interrupt Enable bit
#define TOE_INT_EN_S19_POS 19
//! Field S19 - Interrupt Enable bit
#define TOE_INT_EN_S19_MASK 0x80000u
//! Constant UEN - Disable this bit
#define CONST_TOE_INT_EN_S19_UEN 0x0
//! Constant EN - Enable this bit
#define CONST_TOE_INT_EN_S19_EN 0x1

//! Field S20 - Interrupt Enable bit
#define TOE_INT_EN_S20_POS 20
//! Field S20 - Interrupt Enable bit
#define TOE_INT_EN_S20_MASK 0x100000u
//! Constant UEN - Disable this bit
#define CONST_TOE_INT_EN_S20_UEN 0x0
//! Constant EN - Enable this bit
#define CONST_TOE_INT_EN_S20_EN 0x1

//! Field S21 - Interrupt Enable bit
#define TOE_INT_EN_S21_POS 21
//! Field S21 - Interrupt Enable bit
#define TOE_INT_EN_S21_MASK 0x200000u
//! Constant UEN - Disable this bit
#define CONST_TOE_INT_EN_S21_UEN 0x0
//! Constant EN - Enable this bit
#define CONST_TOE_INT_EN_S21_EN 0x1

//! Field S22 - Interrupt Enable bit
#define TOE_INT_EN_S22_POS 22
//! Field S22 - Interrupt Enable bit
#define TOE_INT_EN_S22_MASK 0x400000u
//! Constant UEN - Disable this bit
#define CONST_TOE_INT_EN_S22_UEN 0x0
//! Constant EN - Enable this bit
#define CONST_TOE_INT_EN_S22_EN 0x1

//! Field S23 - Interrupt Enable bit
#define TOE_INT_EN_S23_POS 23
//! Field S23 - Interrupt Enable bit
#define TOE_INT_EN_S23_MASK 0x800000u
//! Constant UEN - Disable this bit
#define CONST_TOE_INT_EN_S23_UEN 0x0
//! Constant EN - Enable this bit
#define CONST_TOE_INT_EN_S23_EN 0x1

//! @}

//! \defgroup TOE_INT_STAT Register TOE_INT_STAT - Module Interrupt States register
//! @{

//! Register Offset (relative)
#define TOE_INT_STAT 0x104C

//! Register Reset Value
#define TOE_INT_STAT_RST 0x00000000u

//! Field TOE0 - Interrupt States bit
#define TOE_INT_STAT_TOE0_POS 0
//! Field TOE0 - Interrupt States bit
#define TOE_INT_STAT_TOE0_MASK 0x1u
//! Constant UEN - Disabled this bit
#define CONST_TOE_INT_STAT_TOE0_UEN 0x0
//! Constant EN - Enabled this bit
#define CONST_TOE_INT_STAT_TOE0_EN 0x1

//! Field TOE1 - Interrupt States bit
#define TOE_INT_STAT_TOE1_POS 1
//! Field TOE1 - Interrupt States bit
#define TOE_INT_STAT_TOE1_MASK 0x2u
//! Constant UEN - Disabled this bit
#define CONST_TOE_INT_STAT_TOE1_UEN 0x0
//! Constant EN - Enabled this bit
#define CONST_TOE_INT_STAT_TOE1_EN 0x1

//! Field TOE2 - Interrupt States bit
#define TOE_INT_STAT_TOE2_POS 2
//! Field TOE2 - Interrupt States bit
#define TOE_INT_STAT_TOE2_MASK 0x4u
//! Constant UEN - Disabled this bit
#define CONST_TOE_INT_STAT_TOE2_UEN 0x0
//! Constant EN - Enabled this bit
#define CONST_TOE_INT_STAT_TOE2_EN 0x1

//! Field TOE3 - Interrupt States bit
#define TOE_INT_STAT_TOE3_POS 3
//! Field TOE3 - Interrupt States bit
#define TOE_INT_STAT_TOE3_MASK 0x8u
//! Constant UEN - Disabled this bit
#define CONST_TOE_INT_STAT_TOE3_UEN 0x0
//! Constant EN - Enabled this bit
#define CONST_TOE_INT_STAT_TOE3_EN 0x1

//! Field LRO0 - Interrupt States bit
#define TOE_INT_STAT_LRO0_POS 4
//! Field LRO0 - Interrupt States bit
#define TOE_INT_STAT_LRO0_MASK 0x10u
//! Constant UEN - Disabled this bit
#define CONST_TOE_INT_STAT_LRO0_UEN 0x0
//! Constant EN - Enabled this bit
#define CONST_TOE_INT_STAT_LRO0_EN 0x1

//! Field LRO1 - Interrupt States bit
#define TOE_INT_STAT_LRO1_POS 5
//! Field LRO1 - Interrupt States bit
#define TOE_INT_STAT_LRO1_MASK 0x20u
//! Constant UEN - Disabled this bit
#define CONST_TOE_INT_STAT_LRO1_UEN 0x0
//! Constant EN - Enabled this bit
#define CONST_TOE_INT_STAT_LRO1_EN 0x1

//! Field LRO2 - Interrupt States bit
#define TOE_INT_STAT_LRO2_POS 6
//! Field LRO2 - Interrupt States bit
#define TOE_INT_STAT_LRO2_MASK 0x40u
//! Constant UEN - Disabled this bit
#define CONST_TOE_INT_STAT_LRO2_UEN 0x0
//! Constant EN - Enabled this bit
#define CONST_TOE_INT_STAT_LRO2_EN 0x1

//! Field LRO3 - Interrupt States bit
#define TOE_INT_STAT_LRO3_POS 7
//! Field LRO3 - Interrupt States bit
#define TOE_INT_STAT_LRO3_MASK 0x80u
//! Constant UEN - Disabled this bit
#define CONST_TOE_INT_STAT_LRO3_UEN 0x0
//! Constant EN - Enabled this bit
#define CONST_TOE_INT_STAT_LRO3_EN 0x1

//! Field LRO4 - Interrupt States bit
#define TOE_INT_STAT_LRO4_POS 8
//! Field LRO4 - Interrupt States bit
#define TOE_INT_STAT_LRO4_MASK 0x100u
//! Constant UEN - Disabled this bit
#define CONST_TOE_INT_STAT_LRO4_UEN 0x0
//! Constant EN - Enabled this bit
#define CONST_TOE_INT_STAT_LRO4_EN 0x1

//! Field LRO5 - Interrupt States bit
#define TOE_INT_STAT_LRO5_POS 9
//! Field LRO5 - Interrupt States bit
#define TOE_INT_STAT_LRO5_MASK 0x200u
//! Constant UEN - Disabled this bit
#define CONST_TOE_INT_STAT_LRO5_UEN 0x0
//! Constant EN - Enabled this bit
#define CONST_TOE_INT_STAT_LRO5_EN 0x1

//! Field LRO6 - Interrupt States bit
#define TOE_INT_STAT_LRO6_POS 10
//! Field LRO6 - Interrupt States bit
#define TOE_INT_STAT_LRO6_MASK 0x400u
//! Constant UEN - Disabled this bit
#define CONST_TOE_INT_STAT_LRO6_UEN 0x0
//! Constant EN - Enabled this bit
#define CONST_TOE_INT_STAT_LRO6_EN 0x1

//! Field LRO7 - Interrupt States bit
#define TOE_INT_STAT_LRO7_POS 11
//! Field LRO7 - Interrupt States bit
#define TOE_INT_STAT_LRO7_MASK 0x800u
//! Constant UEN - Disabled this bit
#define CONST_TOE_INT_STAT_LRO7_UEN 0x0
//! Constant EN - Enabled this bit
#define CONST_TOE_INT_STAT_LRO7_EN 0x1

//! Field LRO_EXP - Interrupt States bit
#define TOE_INT_STAT_LRO_EXP_POS 12
//! Field LRO_EXP - Interrupt States bit
#define TOE_INT_STAT_LRO_EXP_MASK 0x1000u
//! Constant UEN - Disabled this bit
#define CONST_TOE_INT_STAT_LRO_EXP_UEN 0x0
//! Constant EN - Enabled this bit
#define CONST_TOE_INT_STAT_LRO_EXP_EN 0x1

//! Field S16 - Interrupt States bit
#define TOE_INT_STAT_S16_POS 16
//! Field S16 - Interrupt States bit
#define TOE_INT_STAT_S16_MASK 0x10000u
//! Constant UEN - Disabled this bit
#define CONST_TOE_INT_STAT_S16_UEN 0x0
//! Constant EN - Enabled this bit
#define CONST_TOE_INT_STAT_S16_EN 0x1

//! Field S17 - Interrupt States bit
#define TOE_INT_STAT_S17_POS 17
//! Field S17 - Interrupt States bit
#define TOE_INT_STAT_S17_MASK 0x20000u
//! Constant UEN - Disabled this bit
#define CONST_TOE_INT_STAT_S17_UEN 0x0
//! Constant EN - Enabled this bit
#define CONST_TOE_INT_STAT_S17_EN 0x1

//! Field S18 - Interrupt States bit
#define TOE_INT_STAT_S18_POS 18
//! Field S18 - Interrupt States bit
#define TOE_INT_STAT_S18_MASK 0x40000u
//! Constant UEN - Disabled this bit
#define CONST_TOE_INT_STAT_S18_UEN 0x0
//! Constant EN - Enabled this bit
#define CONST_TOE_INT_STAT_S18_EN 0x1

//! Field S19 - Interrupt States bit
#define TOE_INT_STAT_S19_POS 19
//! Field S19 - Interrupt States bit
#define TOE_INT_STAT_S19_MASK 0x80000u
//! Constant UEN - Disabled this bit
#define CONST_TOE_INT_STAT_S19_UEN 0x0
//! Constant EN - Enabled this bit
#define CONST_TOE_INT_STAT_S19_EN 0x1

//! Field S20 - Interrupt States bit for no match flag overflow
#define TOE_INT_STAT_S20_POS 20
//! Field S20 - Interrupt States bit for no match flag overflow
#define TOE_INT_STAT_S20_MASK 0x100000u
//! Constant UEN - Disabled this bit
#define CONST_TOE_INT_STAT_S20_UEN 0x0
//! Constant EN - Enabled this bit
#define CONST_TOE_INT_STAT_S20_EN 0x1

//! Field S21 - Interrupt States bit for OC flag overflow
#define TOE_INT_STAT_S21_POS 21
//! Field S21 - Interrupt States bit for OC flag overflow
#define TOE_INT_STAT_S21_MASK 0x200000u
//! Constant UEN - Disabled this bit
#define CONST_TOE_INT_STAT_S21_UEN 0x0
//! Constant EN - Enabled this bit
#define CONST_TOE_INT_STAT_S21_EN 0x1

//! Field S22 - Interrupt States bit for exception error flag overflow
#define TOE_INT_STAT_S22_POS 22
//! Field S22 - Interrupt States bit for exception error flag overflow
#define TOE_INT_STAT_S22_MASK 0x400000u
//! Constant UEN - Disabled this bit
#define CONST_TOE_INT_STAT_S22_UEN 0x0
//! Constant EN - Enabled this bit
#define CONST_TOE_INT_STAT_S22_EN 0x1

//! Field S23 - Interrupt States bit for OCP bus error
#define TOE_INT_STAT_S23_POS 23
//! Field S23 - Interrupt States bit for OCP bus error
#define TOE_INT_STAT_S23_MASK 0x800000u
//! Constant UEN - Disabled this bit
#define CONST_TOE_INT_STAT_S23_UEN 0x0
//! Constant EN - Enabled this bit
#define CONST_TOE_INT_STAT_S23_EN 0x1

//! @}

//! \defgroup TSO_DEBUG_CFG Register TSO_DEBUG_CFG
//! @{

//! Register Offset (relative)
#define TSO_DEBUG_CFG 0x1050

//! Register Reset Value
#define TSO_DEBUG_CFG_RST 0x00000000u

//! Field S_FSM - S FSM states
#define TSO_DEBUG_CFG_S_FSM_POS 0
//! Field S_FSM - S FSM states
#define TSO_DEBUG_CFG_S_FSM_MASK 0xFu

//! Field E_FSM - E FSM States
#define TSO_DEBUG_CFG_E_FSM_POS 4
//! Field E_FSM - E FSM States
#define TSO_DEBUG_CFG_E_FSM_MASK 0xF0u

//! Field S_PP_FLAG - S PP FLAG
#define TSO_DEBUG_CFG_S_PP_FLAG_POS 8
//! Field S_PP_FLAG - S PP FLAG
#define TSO_DEBUG_CFG_S_PP_FLAG_MASK 0x100u

//! Field E_PP_FLAG - E PP FLAG
#define TSO_DEBUG_CFG_E_PP_FLAG_POS 9
//! Field E_PP_FLAG - E PP FLAG
#define TSO_DEBUG_CFG_E_PP_FLAG_MASK 0x200u

//! Field PING_VLD - PING Valid
#define TSO_DEBUG_CFG_PING_VLD_POS 10
//! Field PING_VLD - PING Valid
#define TSO_DEBUG_CFG_PING_VLD_MASK 0x400u

//! Field PONG_VLD - PONG Valid
#define TSO_DEBUG_CFG_PONG_VLD_POS 11
//! Field PONG_VLD - PONG Valid
#define TSO_DEBUG_CFG_PONG_VLD_MASK 0x800u

//! Field LAST_VLD - LAST Valid
#define TSO_DEBUG_CFG_LAST_VLD_POS 12
//! Field LAST_VLD - LAST Valid
#define TSO_DEBUG_CFG_LAST_VLD_MASK 0x1000u

//! Field FINAL_VLD - Final valid
#define TSO_DEBUG_CFG_FINAL_VLD_POS 13
//! Field FINAL_VLD - Final valid
#define TSO_DEBUG_CFG_FINAL_VLD_MASK 0x2000u

//! Field CBM_SDES_FLG - CBM Descriptor Flag
#define TSO_DEBUG_CFG_CBM_SDES_FLG_POS 14
//! Field CBM_SDES_FLG - CBM Descriptor Flag
#define TSO_DEBUG_CFG_CBM_SDES_FLG_MASK 0x4000u

//! Field CBM_JDES_FLG - CBM Descriptor Flag
#define TSO_DEBUG_CFG_CBM_JDES_FLG_POS 15
//! Field CBM_JDES_FLG - CBM Descriptor Flag
#define TSO_DEBUG_CFG_CBM_JDES_FLG_MASK 0x8000u

//! Field VER - TSO Version code
#define TSO_DEBUG_CFG_VER_POS 28
//! Field VER - TSO Version code
#define TSO_DEBUG_CFG_VER_MASK 0xF0000000u

//! @}

//! \defgroup TSO_INTL_INT_STAT Register TSO_INTL_INT_STAT - Internal Interrupt Status
//! @{

//! Register Offset (relative)
#define TSO_INTL_INT_STAT 0x1054

//! Register Reset Value
#define TSO_INTL_INT_STAT_RST 0x00000000u

//! Field MCPY0_DONE
#define TSO_INTL_INT_STAT_MCPY0_DONE_POS 0
//! Field MCPY0_DONE
#define TSO_INTL_INT_STAT_MCPY0_DONE_MASK 0x1u

//! Field MCPY1_DONE
#define TSO_INTL_INT_STAT_MCPY1_DONE_POS 1
//! Field MCPY1_DONE
#define TSO_INTL_INT_STAT_MCPY1_DONE_MASK 0x2u

//! Field MCPY2_DONE
#define TSO_INTL_INT_STAT_MCPY2_DONE_POS 2
//! Field MCPY2_DONE
#define TSO_INTL_INT_STAT_MCPY2_DONE_MASK 0x4u

//! Field MCPY3_DONE
#define TSO_INTL_INT_STAT_MCPY3_DONE_POS 3
//! Field MCPY3_DONE
#define TSO_INTL_INT_STAT_MCPY3_DONE_MASK 0x8u

//! Field MCPY0_CMD_ERR
#define TSO_INTL_INT_STAT_MCPY0_CMD_ERR_POS 4
//! Field MCPY0_CMD_ERR
#define TSO_INTL_INT_STAT_MCPY0_CMD_ERR_MASK 0x10u

//! Field MCPY1_CMD_ERR
#define TSO_INTL_INT_STAT_MCPY1_CMD_ERR_POS 5
//! Field MCPY1_CMD_ERR
#define TSO_INTL_INT_STAT_MCPY1_CMD_ERR_MASK 0x20u

//! Field MCPY2_CMD_ERR
#define TSO_INTL_INT_STAT_MCPY2_CMD_ERR_POS 6
//! Field MCPY2_CMD_ERR
#define TSO_INTL_INT_STAT_MCPY2_CMD_ERR_MASK 0x40u

//! Field MCPY3_CMD_ERR
#define TSO_INTL_INT_STAT_MCPY3_CMD_ERR_POS 7
//! Field MCPY3_CMD_ERR
#define TSO_INTL_INT_STAT_MCPY3_CMD_ERR_MASK 0x80u

//! Field MCPY0_LEN_ERR
#define TSO_INTL_INT_STAT_MCPY0_LEN_ERR_POS 8
//! Field MCPY0_LEN_ERR
#define TSO_INTL_INT_STAT_MCPY0_LEN_ERR_MASK 0x100u

//! Field MCPY1_LEN_ERR
#define TSO_INTL_INT_STAT_MCPY1_LEN_ERR_POS 9
//! Field MCPY1_LEN_ERR
#define TSO_INTL_INT_STAT_MCPY1_LEN_ERR_MASK 0x200u

//! Field MCPY2_LEN_ERR
#define TSO_INTL_INT_STAT_MCPY2_LEN_ERR_POS 10
//! Field MCPY2_LEN_ERR
#define TSO_INTL_INT_STAT_MCPY2_LEN_ERR_MASK 0x400u

//! Field MCPY3_LEN_ERR
#define TSO_INTL_INT_STAT_MCPY3_LEN_ERR_POS 11
//! Field MCPY3_LEN_ERR
#define TSO_INTL_INT_STAT_MCPY3_LEN_ERR_MASK 0x800u

//! Field TSO0_TCP_ERR
#define TSO_INTL_INT_STAT_TSO0_TCP_ERR_POS 12
//! Field TSO0_TCP_ERR
#define TSO_INTL_INT_STAT_TSO0_TCP_ERR_MASK 0x1000u

//! Field TSO1_TCP_ERR
#define TSO_INTL_INT_STAT_TSO1_TCP_ERR_POS 13
//! Field TSO1_TCP_ERR
#define TSO_INTL_INT_STAT_TSO1_TCP_ERR_MASK 0x2000u

//! Field TSO2_TCP_ERR
#define TSO_INTL_INT_STAT_TSO2_TCP_ERR_POS 14
//! Field TSO2_TCP_ERR
#define TSO_INTL_INT_STAT_TSO2_TCP_ERR_MASK 0x4000u

//! Field TSO3_TCP_ERR
#define TSO_INTL_INT_STAT_TSO3_TCP_ERR_POS 15
//! Field TSO3_TCP_ERR
#define TSO_INTL_INT_STAT_TSO3_TCP_ERR_MASK 0x8000u

//! Field TSO0_BUS_ERR
#define TSO_INTL_INT_STAT_TSO0_BUS_ERR_POS 16
//! Field TSO0_BUS_ERR
#define TSO_INTL_INT_STAT_TSO0_BUS_ERR_MASK 0x10000u

//! Field TSO1_BUS_ERR
#define TSO_INTL_INT_STAT_TSO1_BUS_ERR_POS 17
//! Field TSO1_BUS_ERR
#define TSO_INTL_INT_STAT_TSO1_BUS_ERR_MASK 0x20000u

//! Field TSO2_BUS_ERR
#define TSO_INTL_INT_STAT_TSO2_BUS_ERR_POS 18
//! Field TSO2_BUS_ERR
#define TSO_INTL_INT_STAT_TSO2_BUS_ERR_MASK 0x40000u

//! Field TSO3_BUS_ERR
#define TSO_INTL_INT_STAT_TSO3_BUS_ERR_POS 19
//! Field TSO3_BUS_ERR
#define TSO_INTL_INT_STAT_TSO3_BUS_ERR_MASK 0x80000u

//! @}

//! \defgroup TSO_INTL_INT_EN Register TSO_INTL_INT_EN - Internal Interrupt Status
//! @{

//! Register Offset (relative)
#define TSO_INTL_INT_EN 0x1058

//! Register Reset Value
#define TSO_INTL_INT_EN_RST 0x00000000u

//! Field MCPY0_DONE
#define TSO_INTL_INT_EN_MCPY0_DONE_POS 0
//! Field MCPY0_DONE
#define TSO_INTL_INT_EN_MCPY0_DONE_MASK 0x1u

//! Field MCPY1_DONE
#define TSO_INTL_INT_EN_MCPY1_DONE_POS 1
//! Field MCPY1_DONE
#define TSO_INTL_INT_EN_MCPY1_DONE_MASK 0x2u

//! Field MCPY2_DONE
#define TSO_INTL_INT_EN_MCPY2_DONE_POS 2
//! Field MCPY2_DONE
#define TSO_INTL_INT_EN_MCPY2_DONE_MASK 0x4u

//! Field MCPY3_DONE
#define TSO_INTL_INT_EN_MCPY3_DONE_POS 3
//! Field MCPY3_DONE
#define TSO_INTL_INT_EN_MCPY3_DONE_MASK 0x8u

//! Field MCPY0_CMD_ERR
#define TSO_INTL_INT_EN_MCPY0_CMD_ERR_POS 4
//! Field MCPY0_CMD_ERR
#define TSO_INTL_INT_EN_MCPY0_CMD_ERR_MASK 0x10u

//! Field MCPY1_CMD_ERR
#define TSO_INTL_INT_EN_MCPY1_CMD_ERR_POS 5
//! Field MCPY1_CMD_ERR
#define TSO_INTL_INT_EN_MCPY1_CMD_ERR_MASK 0x20u

//! Field MCPY2_CMD_ERR
#define TSO_INTL_INT_EN_MCPY2_CMD_ERR_POS 6
//! Field MCPY2_CMD_ERR
#define TSO_INTL_INT_EN_MCPY2_CMD_ERR_MASK 0x40u

//! Field MCPY3_CMD_ERR
#define TSO_INTL_INT_EN_MCPY3_CMD_ERR_POS 7
//! Field MCPY3_CMD_ERR
#define TSO_INTL_INT_EN_MCPY3_CMD_ERR_MASK 0x80u

//! Field MCPY0_LEN_ERR
#define TSO_INTL_INT_EN_MCPY0_LEN_ERR_POS 8
//! Field MCPY0_LEN_ERR
#define TSO_INTL_INT_EN_MCPY0_LEN_ERR_MASK 0x100u

//! Field MCPY1_LEN_ERR
#define TSO_INTL_INT_EN_MCPY1_LEN_ERR_POS 9
//! Field MCPY1_LEN_ERR
#define TSO_INTL_INT_EN_MCPY1_LEN_ERR_MASK 0x200u

//! Field MCPY2_LEN_ERR
#define TSO_INTL_INT_EN_MCPY2_LEN_ERR_POS 10
//! Field MCPY2_LEN_ERR
#define TSO_INTL_INT_EN_MCPY2_LEN_ERR_MASK 0x400u

//! Field MCPY3_LEN_ERR
#define TSO_INTL_INT_EN_MCPY3_LEN_ERR_POS 11
//! Field MCPY3_LEN_ERR
#define TSO_INTL_INT_EN_MCPY3_LEN_ERR_MASK 0x800u

//! Field TSO0_TCP_ERR
#define TSO_INTL_INT_EN_TSO0_TCP_ERR_POS 12
//! Field TSO0_TCP_ERR
#define TSO_INTL_INT_EN_TSO0_TCP_ERR_MASK 0x1000u

//! Field TSO1_TCP_ERR
#define TSO_INTL_INT_EN_TSO1_TCP_ERR_POS 13
//! Field TSO1_TCP_ERR
#define TSO_INTL_INT_EN_TSO1_TCP_ERR_MASK 0x2000u

//! Field TSO2_TCP_ERR
#define TSO_INTL_INT_EN_TSO2_TCP_ERR_POS 14
//! Field TSO2_TCP_ERR
#define TSO_INTL_INT_EN_TSO2_TCP_ERR_MASK 0x4000u

//! Field TSO3_TCP_ERR
#define TSO_INTL_INT_EN_TSO3_TCP_ERR_POS 15
//! Field TSO3_TCP_ERR
#define TSO_INTL_INT_EN_TSO3_TCP_ERR_MASK 0x8000u

//! Field TSO0_BUS_ERR
#define TSO_INTL_INT_EN_TSO0_BUS_ERR_POS 16
//! Field TSO0_BUS_ERR
#define TSO_INTL_INT_EN_TSO0_BUS_ERR_MASK 0x10000u

//! Field TSO1_BUS_ERR
#define TSO_INTL_INT_EN_TSO1_BUS_ERR_POS 17
//! Field TSO1_BUS_ERR
#define TSO_INTL_INT_EN_TSO1_BUS_ERR_MASK 0x20000u

//! Field TSO2_BUS_ERR
#define TSO_INTL_INT_EN_TSO2_BUS_ERR_POS 18
//! Field TSO2_BUS_ERR
#define TSO_INTL_INT_EN_TSO2_BUS_ERR_MASK 0x40000u

//! Field TSO3_BUS_ERR
#define TSO_INTL_INT_EN_TSO3_BUS_ERR_POS 19
//! Field TSO3_BUS_ERR
#define TSO_INTL_INT_EN_TSO3_BUS_ERR_MASK 0x80000u

//! @}

//! \defgroup TSO_INTL_INT_MASK Register TSO_INTL_INT_MASK - Internal Interrupt Status
//! @{

//! Register Offset (relative)
#define TSO_INTL_INT_MASK 0x105C

//! Register Reset Value
#define TSO_INTL_INT_MASK_RST 0x00000000u

//! Field MCPY0_DONE
#define TSO_INTL_INT_MASK_MCPY0_DONE_POS 0
//! Field MCPY0_DONE
#define TSO_INTL_INT_MASK_MCPY0_DONE_MASK 0x1u

//! Field MCPY1_DONE
#define TSO_INTL_INT_MASK_MCPY1_DONE_POS 1
//! Field MCPY1_DONE
#define TSO_INTL_INT_MASK_MCPY1_DONE_MASK 0x2u

//! Field MCPY2_DONE
#define TSO_INTL_INT_MASK_MCPY2_DONE_POS 2
//! Field MCPY2_DONE
#define TSO_INTL_INT_MASK_MCPY2_DONE_MASK 0x4u

//! Field MCPY3_DONE
#define TSO_INTL_INT_MASK_MCPY3_DONE_POS 3
//! Field MCPY3_DONE
#define TSO_INTL_INT_MASK_MCPY3_DONE_MASK 0x8u

//! Field MCPY0_CMD_ERR
#define TSO_INTL_INT_MASK_MCPY0_CMD_ERR_POS 4
//! Field MCPY0_CMD_ERR
#define TSO_INTL_INT_MASK_MCPY0_CMD_ERR_MASK 0x10u

//! Field MCPY1_CMD_ERR
#define TSO_INTL_INT_MASK_MCPY1_CMD_ERR_POS 5
//! Field MCPY1_CMD_ERR
#define TSO_INTL_INT_MASK_MCPY1_CMD_ERR_MASK 0x20u

//! Field MCPY2_CMD_ERR
#define TSO_INTL_INT_MASK_MCPY2_CMD_ERR_POS 6
//! Field MCPY2_CMD_ERR
#define TSO_INTL_INT_MASK_MCPY2_CMD_ERR_MASK 0x40u

//! Field MCPY3_CMD_ERR
#define TSO_INTL_INT_MASK_MCPY3_CMD_ERR_POS 7
//! Field MCPY3_CMD_ERR
#define TSO_INTL_INT_MASK_MCPY3_CMD_ERR_MASK 0x80u

//! Field MCPY0_LEN_ERR
#define TSO_INTL_INT_MASK_MCPY0_LEN_ERR_POS 8
//! Field MCPY0_LEN_ERR
#define TSO_INTL_INT_MASK_MCPY0_LEN_ERR_MASK 0x100u

//! Field MCPY1_LEN_ERR
#define TSO_INTL_INT_MASK_MCPY1_LEN_ERR_POS 9
//! Field MCPY1_LEN_ERR
#define TSO_INTL_INT_MASK_MCPY1_LEN_ERR_MASK 0x200u

//! Field MCPY2_LEN_ERR
#define TSO_INTL_INT_MASK_MCPY2_LEN_ERR_POS 10
//! Field MCPY2_LEN_ERR
#define TSO_INTL_INT_MASK_MCPY2_LEN_ERR_MASK 0x400u

//! Field MCPY3_LEN_ERR
#define TSO_INTL_INT_MASK_MCPY3_LEN_ERR_POS 11
//! Field MCPY3_LEN_ERR
#define TSO_INTL_INT_MASK_MCPY3_LEN_ERR_MASK 0x800u

//! Field TSO0_TCP_ERR
#define TSO_INTL_INT_MASK_TSO0_TCP_ERR_POS 12
//! Field TSO0_TCP_ERR
#define TSO_INTL_INT_MASK_TSO0_TCP_ERR_MASK 0x1000u

//! Field TSO1_TCP_ERR
#define TSO_INTL_INT_MASK_TSO1_TCP_ERR_POS 13
//! Field TSO1_TCP_ERR
#define TSO_INTL_INT_MASK_TSO1_TCP_ERR_MASK 0x2000u

//! Field TSO2_TCP_ERR
#define TSO_INTL_INT_MASK_TSO2_TCP_ERR_POS 14
//! Field TSO2_TCP_ERR
#define TSO_INTL_INT_MASK_TSO2_TCP_ERR_MASK 0x4000u

//! Field TSO3_TCP_ERR
#define TSO_INTL_INT_MASK_TSO3_TCP_ERR_POS 15
//! Field TSO3_TCP_ERR
#define TSO_INTL_INT_MASK_TSO3_TCP_ERR_MASK 0x8000u

//! Field TSO0_BUS_ERR
#define TSO_INTL_INT_MASK_TSO0_BUS_ERR_POS 16
//! Field TSO0_BUS_ERR
#define TSO_INTL_INT_MASK_TSO0_BUS_ERR_MASK 0x10000u

//! Field TSO1_BUS_ERR
#define TSO_INTL_INT_MASK_TSO1_BUS_ERR_POS 17
//! Field TSO1_BUS_ERR
#define TSO_INTL_INT_MASK_TSO1_BUS_ERR_MASK 0x20000u

//! Field TSO2_BUS_ERR
#define TSO_INTL_INT_MASK_TSO2_BUS_ERR_POS 18
//! Field TSO2_BUS_ERR
#define TSO_INTL_INT_MASK_TSO2_BUS_ERR_MASK 0x40000u

//! Field TSO3_BUS_ERR
#define TSO_INTL_INT_MASK_TSO3_BUS_ERR_POS 19
//! Field TSO3_BUS_ERR
#define TSO_INTL_INT_MASK_TSO3_BUS_ERR_MASK 0x80000u

//! @}

//! \defgroup TSO_DBG_HPIN_D_RX0 Register TSO_DBG_HPIN_D_RX0
//! @{

//! Register Offset (relative)
#define TSO_DBG_HPIN_D_RX0 0x1060

//! Register Reset Value
#define TSO_DBG_HPIN_D_RX0_RST 0x00000000u

//! Field DOWRD0 - Descriptor Word 0
#define TSO_DBG_HPIN_D_RX0_DOWRD0_POS 0
//! Field DOWRD0 - Descriptor Word 0
#define TSO_DBG_HPIN_D_RX0_DOWRD0_MASK 0xFFFFFFFFu

//! @}

//! \defgroup TSO_DBG_HPIN_D_RX1 Register TSO_DBG_HPIN_D_RX1
//! @{

//! Register Offset (relative)
#define TSO_DBG_HPIN_D_RX1 0x1064

//! Register Reset Value
#define TSO_DBG_HPIN_D_RX1_RST 0x00000000u

//! Field DWORD1 - Descriptor 1
#define TSO_DBG_HPIN_D_RX1_DWORD1_POS 0
//! Field DWORD1 - Descriptor 1
#define TSO_DBG_HPIN_D_RX1_DWORD1_MASK 0xFFFFFFFFu

//! @}

//! \defgroup TSO_DBG_DPIN_D_RX0 Register TSO_DBG_DPIN_D_RX0
//! @{

//! Register Offset (relative)
#define TSO_DBG_DPIN_D_RX0 0x1068

//! Register Reset Value
#define TSO_DBG_DPIN_D_RX0_RST 0x00000000u

//! Field DOWRD0 - Descriptor Word 0
#define TSO_DBG_DPIN_D_RX0_DOWRD0_POS 0
//! Field DOWRD0 - Descriptor Word 0
#define TSO_DBG_DPIN_D_RX0_DOWRD0_MASK 0xFFFFFFFFu

//! @}

//! \defgroup TSO_DBG_DPIN_D_RX1 Register TSO_DBG_DPIN_D_RX1
//! @{

//! Register Offset (relative)
#define TSO_DBG_DPIN_D_RX1 0x106C

//! Register Reset Value
#define TSO_DBG_DPIN_D_RX1_RST 0x00000000u

//! Field DWORD1 - Descriptor 1
#define TSO_DBG_DPIN_D_RX1_DWORD1_POS 0
//! Field DWORD1 - Descriptor 1
#define TSO_DBG_DPIN_D_RX1_DWORD1_MASK 0xFFFFFFFFu

//! @}

//! \defgroup TSO_DBG_HPIN_D_TX0 Register TSO_DBG_HPIN_D_TX0
//! @{

//! Register Offset (relative)
#define TSO_DBG_HPIN_D_TX0 0x1070

//! Register Reset Value
#define TSO_DBG_HPIN_D_TX0_RST 0x00000000u

//! Field DOWRD0 - Descriptor Word 0
#define TSO_DBG_HPIN_D_TX0_DOWRD0_POS 0
//! Field DOWRD0 - Descriptor Word 0
#define TSO_DBG_HPIN_D_TX0_DOWRD0_MASK 0xFFFFFFFFu

//! @}

//! \defgroup TSO_DBG_HPIN_D_TX1 Register TSO_DBG_HPIN_D_TX1
//! @{

//! Register Offset (relative)
#define TSO_DBG_HPIN_D_TX1 0x1074

//! Register Reset Value
#define TSO_DBG_HPIN_D_TX1_RST 0x00000000u

//! Field DWORD1 - Descriptor 1
#define TSO_DBG_HPIN_D_TX1_DWORD1_POS 0
//! Field DWORD1 - Descriptor 1
#define TSO_DBG_HPIN_D_TX1_DWORD1_MASK 0xFFFFFFFFu

//! @}

//! \defgroup TSO_DBG_DPIN_D_TX0 Register TSO_DBG_DPIN_D_TX0
//! @{

//! Register Offset (relative)
#define TSO_DBG_DPIN_D_TX0 0x1078

//! Register Reset Value
#define TSO_DBG_DPIN_D_TX0_RST 0x00000000u

//! Field DOWRD0 - Descriptor Word 0
#define TSO_DBG_DPIN_D_TX0_DOWRD0_POS 0
//! Field DOWRD0 - Descriptor Word 0
#define TSO_DBG_DPIN_D_TX0_DOWRD0_MASK 0xFFFFFFFFu

//! @}

//! \defgroup TSO_DBG_DPIN_D_TX1 Register TSO_DBG_DPIN_D_TX1
//! @{

//! Register Offset (relative)
#define TSO_DBG_DPIN_D_TX1 0x107C

//! Register Reset Value
#define TSO_DBG_DPIN_D_TX1_RST 0x00000000u

//! Field DWORD1 - Descriptor 1
#define TSO_DBG_DPIN_D_TX1_DWORD1_POS 0
//! Field DWORD1 - Descriptor 1
#define TSO_DBG_DPIN_D_TX1_DWORD1_MASK 0xFFFFFFFFu

//! @}

//! \defgroup TSO_DBG_HPON_D_RX0 Register TSO_DBG_HPON_D_RX0
//! @{

//! Register Offset (relative)
#define TSO_DBG_HPON_D_RX0 0x1080

//! Register Reset Value
#define TSO_DBG_HPON_D_RX0_RST 0x00000000u

//! Field DOWRD0 - Descriptor Word 0
#define TSO_DBG_HPON_D_RX0_DOWRD0_POS 0
//! Field DOWRD0 - Descriptor Word 0
#define TSO_DBG_HPON_D_RX0_DOWRD0_MASK 0xFFFFFFFFu

//! @}

//! \defgroup TSO_DBG_HPON_D_RX1 Register TSO_DBG_HPON_D_RX1
//! @{

//! Register Offset (relative)
#define TSO_DBG_HPON_D_RX1 0x1084

//! Register Reset Value
#define TSO_DBG_HPON_D_RX1_RST 0x00000000u

//! Field DWORD1 - Descriptor 1
#define TSO_DBG_HPON_D_RX1_DWORD1_POS 0
//! Field DWORD1 - Descriptor 1
#define TSO_DBG_HPON_D_RX1_DWORD1_MASK 0xFFFFFFFFu

//! @}

//! \defgroup TSO_DBG_DPON_D_RX0 Register TSO_DBG_DPON_D_RX0
//! @{

//! Register Offset (relative)
#define TSO_DBG_DPON_D_RX0 0x1088

//! Register Reset Value
#define TSO_DBG_DPON_D_RX0_RST 0x00000000u

//! Field DOWRD0 - Descriptor Word 0
#define TSO_DBG_DPON_D_RX0_DOWRD0_POS 0
//! Field DOWRD0 - Descriptor Word 0
#define TSO_DBG_DPON_D_RX0_DOWRD0_MASK 0xFFFFFFFFu

//! @}

//! \defgroup TSO_DBG_DPON_D_RX1 Register TSO_DBG_DPON_D_RX1
//! @{

//! Register Offset (relative)
#define TSO_DBG_DPON_D_RX1 0x108C

//! Register Reset Value
#define TSO_DBG_DPON_D_RX1_RST 0x00000000u

//! Field DWORD1 - Descriptor 1
#define TSO_DBG_DPON_D_RX1_DWORD1_POS 0
//! Field DWORD1 - Descriptor 1
#define TSO_DBG_DPON_D_RX1_DWORD1_MASK 0xFFFFFFFFu

//! @}

//! \defgroup TSO_DBG_HPON_D_TX0 Register TSO_DBG_HPON_D_TX0
//! @{

//! Register Offset (relative)
#define TSO_DBG_HPON_D_TX0 0x1090

//! Register Reset Value
#define TSO_DBG_HPON_D_TX0_RST 0x00000000u

//! Field DOWRD0 - Descriptor Word 0
#define TSO_DBG_HPON_D_TX0_DOWRD0_POS 0
//! Field DOWRD0 - Descriptor Word 0
#define TSO_DBG_HPON_D_TX0_DOWRD0_MASK 0xFFFFFFFFu

//! @}

//! \defgroup TSO_DBG_HPON_D_TX1 Register TSO_DBG_HPON_D_TX1
//! @{

//! Register Offset (relative)
#define TSO_DBG_HPON_D_TX1 0x1094

//! Register Reset Value
#define TSO_DBG_HPON_D_TX1_RST 0x00000000u

//! Field DWORD1 - Descriptor 1
#define TSO_DBG_HPON_D_TX1_DWORD1_POS 0
//! Field DWORD1 - Descriptor 1
#define TSO_DBG_HPON_D_TX1_DWORD1_MASK 0xFFFFFFFFu

//! @}

//! \defgroup TSO_DBG_DPON_D_TX0 Register TSO_DBG_DPON_D_TX0
//! @{

//! Register Offset (relative)
#define TSO_DBG_DPON_D_TX0 0x1098

//! Register Reset Value
#define TSO_DBG_DPON_D_TX0_RST 0x00000000u

//! Field DOWRD0 - Descriptor Word 0
#define TSO_DBG_DPON_D_TX0_DOWRD0_POS 0
//! Field DOWRD0 - Descriptor Word 0
#define TSO_DBG_DPON_D_TX0_DOWRD0_MASK 0xFFFFFFFFu

//! @}

//! \defgroup TSO_DBG_DPON_D_TX1 Register TSO_DBG_DPON_D_TX1
//! @{

//! Register Offset (relative)
#define TSO_DBG_DPON_D_TX1 0x109C

//! Register Reset Value
#define TSO_DBG_DPON_D_TX1_RST 0x00000000u

//! Field DWORD1 - Descriptor 1
#define TSO_DBG_DPON_D_TX1_DWORD1_POS 0
//! Field DWORD1 - Descriptor 1
#define TSO_DBG_DPON_D_TX1_DWORD1_MASK 0xFFFFFFFFu

//! @}

//! \defgroup TSO_DBG_EPIN_D_RX0 Register TSO_DBG_EPIN_D_RX0
//! @{

//! Register Offset (relative)
#define TSO_DBG_EPIN_D_RX0 0x10A0

//! Register Reset Value
#define TSO_DBG_EPIN_D_RX0_RST 0x00000000u

//! Field DOWRD0 - Descriptor Word 0
#define TSO_DBG_EPIN_D_RX0_DOWRD0_POS 0
//! Field DOWRD0 - Descriptor Word 0
#define TSO_DBG_EPIN_D_RX0_DOWRD0_MASK 0xFFFFFFFFu

//! @}

//! \defgroup TSO_DBG_EPIN_D_RX1 Register TSO_DBG_EPIN_D_RX1
//! @{

//! Register Offset (relative)
#define TSO_DBG_EPIN_D_RX1 0x10A4

//! Register Reset Value
#define TSO_DBG_EPIN_D_RX1_RST 0x00000000u

//! Field DWORD1 - Descriptor 1
#define TSO_DBG_EPIN_D_RX1_DWORD1_POS 0
//! Field DWORD1 - Descriptor 1
#define TSO_DBG_EPIN_D_RX1_DWORD1_MASK 0xFFFFFFFFu

//! @}

//! \defgroup TSO_DBG_EPIN_D_TX0 Register TSO_DBG_EPIN_D_TX0
//! @{

//! Register Offset (relative)
#define TSO_DBG_EPIN_D_TX0 0x10A8

//! Register Reset Value
#define TSO_DBG_EPIN_D_TX0_RST 0x00000000u

//! Field DOWRD0 - Descriptor Word 0
#define TSO_DBG_EPIN_D_TX0_DOWRD0_POS 0
//! Field DOWRD0 - Descriptor Word 0
#define TSO_DBG_EPIN_D_TX0_DOWRD0_MASK 0xFFFFFFFFu

//! @}

//! \defgroup TSO_DBG_EPIN_D_TX1 Register TSO_DBG_EPIN_D_TX1
//! @{

//! Register Offset (relative)
#define TSO_DBG_EPIN_D_TX1 0x10AC

//! Register Reset Value
#define TSO_DBG_EPIN_D_TX1_RST 0x00000000u

//! Field DWORD1 - Descriptor 1
#define TSO_DBG_EPIN_D_TX1_DWORD1_POS 0
//! Field DWORD1 - Descriptor 1
#define TSO_DBG_EPIN_D_TX1_DWORD1_MASK 0xFFFFFFFFu

//! @}

//! \defgroup TSO_DBG_EPON_D_RX0 Register TSO_DBG_EPON_D_RX0
//! @{

//! Register Offset (relative)
#define TSO_DBG_EPON_D_RX0 0x10B0

//! Register Reset Value
#define TSO_DBG_EPON_D_RX0_RST 0x00000000u

//! Field DOWRD0 - Descriptor Word 0
#define TSO_DBG_EPON_D_RX0_DOWRD0_POS 0
//! Field DOWRD0 - Descriptor Word 0
#define TSO_DBG_EPON_D_RX0_DOWRD0_MASK 0xFFFFFFFFu

//! @}

//! \defgroup TSO_DBG_EPON_D_RX1 Register TSO_DBG_EPON_D_RX1
//! @{

//! Register Offset (relative)
#define TSO_DBG_EPON_D_RX1 0x10B4

//! Register Reset Value
#define TSO_DBG_EPON_D_RX1_RST 0x00000000u

//! Field DWORD1 - Descriptor 1
#define TSO_DBG_EPON_D_RX1_DWORD1_POS 0
//! Field DWORD1 - Descriptor 1
#define TSO_DBG_EPON_D_RX1_DWORD1_MASK 0xFFFFFFFFu

//! @}

//! \defgroup TSO_DBG_EPON_D_TX0 Register TSO_DBG_EPON_D_TX0
//! @{

//! Register Offset (relative)
#define TSO_DBG_EPON_D_TX0 0x10B8

//! Register Reset Value
#define TSO_DBG_EPON_D_TX0_RST 0x00000000u

//! Field DOWRD0 - Descriptor Word 0
#define TSO_DBG_EPON_D_TX0_DOWRD0_POS 0
//! Field DOWRD0 - Descriptor Word 0
#define TSO_DBG_EPON_D_TX0_DOWRD0_MASK 0xFFFFFFFFu

//! @}

//! \defgroup TSO_DBG_EPON_D_TX1 Register TSO_DBG_EPON_D_TX1
//! @{

//! Register Offset (relative)
#define TSO_DBG_EPON_D_TX1 0x10BC

//! Register Reset Value
#define TSO_DBG_EPON_D_TX1_RST 0x00000000u

//! Field DWORD1 - Descriptor 1
#define TSO_DBG_EPON_D_TX1_DWORD1_POS 0
//! Field DWORD1 - Descriptor 1
#define TSO_DBG_EPON_D_TX1_DWORD1_MASK 0xFFFFFFFFu

//! @}

//! \defgroup TSO_DBG_OCP_RDATA_0 Register TSO_DBG_OCP_RDATA_0
//! @{

//! Register Offset (relative)
#define TSO_DBG_OCP_RDATA_0 0x10C0

//! Register Reset Value
#define TSO_DBG_OCP_RDATA_0_RST 0x00000000u

//! Field READ_DATA - OCP Master read data
#define TSO_DBG_OCP_RDATA_0_READ_DATA_POS 0
//! Field READ_DATA - OCP Master read data
#define TSO_DBG_OCP_RDATA_0_READ_DATA_MASK 0xFFFFFFFFu

//! @}

//! \defgroup TSO_DBG_OCP_RDATA_1 Register TSO_DBG_OCP_RDATA_1
//! @{

//! Register Offset (relative)
#define TSO_DBG_OCP_RDATA_1 0x10C4

//! Register Reset Value
#define TSO_DBG_OCP_RDATA_1_RST 0x00000000u

//! Field READ_DATA - OCP Master read data
#define TSO_DBG_OCP_RDATA_1_READ_DATA_POS 0
//! Field READ_DATA - OCP Master read data
#define TSO_DBG_OCP_RDATA_1_READ_DATA_MASK 0xFFFFFFFFu

//! @}

//! \defgroup TSO_DBG_OCP_RDATA_2 Register TSO_DBG_OCP_RDATA_2
//! @{

//! Register Offset (relative)
#define TSO_DBG_OCP_RDATA_2 0x10C8

//! Register Reset Value
#define TSO_DBG_OCP_RDATA_2_RST 0x00000000u

//! Field READ_DATA - OCP Master read data
#define TSO_DBG_OCP_RDATA_2_READ_DATA_POS 0
//! Field READ_DATA - OCP Master read data
#define TSO_DBG_OCP_RDATA_2_READ_DATA_MASK 0xFFFFFFFFu

//! @}

//! \defgroup TSO_DBG_OCP_RDATA_3 Register TSO_DBG_OCP_RDATA_3
//! @{

//! Register Offset (relative)
#define TSO_DBG_OCP_RDATA_3 0x10CC

//! Register Reset Value
#define TSO_DBG_OCP_RDATA_3_RST 0x00000000u

//! Field READ_DATA - OCP Master read data
#define TSO_DBG_OCP_RDATA_3_READ_DATA_POS 0
//! Field READ_DATA - OCP Master read data
#define TSO_DBG_OCP_RDATA_3_READ_DATA_MASK 0xFFFFFFFFu

//! @}

//! \defgroup LRO_DBG_OCP_RDATA_0 Register LRO_DBG_OCP_RDATA_0
//! @{

//! Register Offset (relative)
#define LRO_DBG_OCP_RDATA_0 0x10D0

//! Register Reset Value
#define LRO_DBG_OCP_RDATA_0_RST 0x00000000u

//! Field READ_DATA - OCP Master read data
#define LRO_DBG_OCP_RDATA_0_READ_DATA_POS 0
//! Field READ_DATA - OCP Master read data
#define LRO_DBG_OCP_RDATA_0_READ_DATA_MASK 0xFFFFFFFFu

//! @}

//! \defgroup LRO_DBG_OCP_RDATA_1 Register LRO_DBG_OCP_RDATA_1
//! @{

//! Register Offset (relative)
#define LRO_DBG_OCP_RDATA_1 0x10D4

//! Register Reset Value
#define LRO_DBG_OCP_RDATA_1_RST 0x00000000u

//! Field READ_DATA - OCP Master read data
#define LRO_DBG_OCP_RDATA_1_READ_DATA_POS 0
//! Field READ_DATA - OCP Master read data
#define LRO_DBG_OCP_RDATA_1_READ_DATA_MASK 0xFFFFFFFFu

//! @}

//! \defgroup LRO_DBG_OCP_RDATA_2 Register LRO_DBG_OCP_RDATA_2
//! @{

//! Register Offset (relative)
#define LRO_DBG_OCP_RDATA_2 0x10D8

//! Register Reset Value
#define LRO_DBG_OCP_RDATA_2_RST 0x00000000u

//! Field READ_DATA - OCP Master read data
#define LRO_DBG_OCP_RDATA_2_READ_DATA_POS 0
//! Field READ_DATA - OCP Master read data
#define LRO_DBG_OCP_RDATA_2_READ_DATA_MASK 0xFFFFFFFFu

//! @}

//! \defgroup LRO_DBG_OCP_RDATA_3 Register LRO_DBG_OCP_RDATA_3
//! @{

//! Register Offset (relative)
#define LRO_DBG_OCP_RDATA_3 0x10DC

//! Register Reset Value
#define LRO_DBG_OCP_RDATA_3_RST 0x00000000u

//! Field READ_DATA - OCP Master read data
#define LRO_DBG_OCP_RDATA_3_READ_DATA_POS 0
//! Field READ_DATA - OCP Master read data
#define LRO_DBG_OCP_RDATA_3_READ_DATA_MASK 0xFFFFFFFFu

//! @}

//! \defgroup LRO_DBG_INFO Register LRO_DBG_INFO
//! @{

//! Register Offset (relative)
#define LRO_DBG_INFO 0x10E0

//! Register Reset Value
#define LRO_DBG_INFO_RST 0x00000000u

//! Field LRO_FSM - LRO FSM
#define LRO_DBG_INFO_LRO_FSM_POS 0
//! Field LRO_FSM - LRO FSM
#define LRO_DBG_INFO_LRO_FSM_MASK 0x1Fu

//! Field SRAM_FSM - SRAM FSM
#define LRO_DBG_INFO_SRAM_FSM_POS 5
//! Field SRAM_FSM - SRAM FSM
#define LRO_DBG_INFO_SRAM_FSM_MASK 0xE0u

//! Field OCPM - OCPM Debug
#define LRO_DBG_INFO_OCPM_POS 8
//! Field OCPM - OCPM Debug
#define LRO_DBG_INFO_OCPM_MASK 0x700u

//! @}

//! \defgroup TOE_MEM_BYPASS Register TOE_MEM_BYPASS
//! @{

//! Register Offset (relative)
#define TOE_MEM_BYPASS 0x10E4

//! Register Reset Value
#define TOE_MEM_BYPASS_RST 0x00000000u

//! Field LS_MEM_BYPASS - control the MEMORY Access
#define TOE_MEM_BYPASS_LS_MEM_BYPASS_POS 0
//! Field LS_MEM_BYPASS - control the MEMORY Access
#define TOE_MEM_BYPASS_LS_MEM_BYPASS_MASK 0x1u
//! Constant NOBYP - not bypass
#define CONST_TOE_MEM_BYPASS_LS_MEM_BYPASS_NOBYP 0x0
//! Constant BYP - Bypassed for debugging
#define CONST_TOE_MEM_BYPASS_LS_MEM_BYPASS_BYP 0x1

//! @}

//! \defgroup TSO_SPARE_TIE0 Register TSO_SPARE_TIE0
//! @{

//! Register Offset (relative)
#define TSO_SPARE_TIE0 0x10E8

//! Register Reset Value
#define TSO_SPARE_TIE0_RST 0x00000000u

//! Field TSO_SPARE - SPARE register
#define TSO_SPARE_TIE0_TSO_SPARE_POS 0
//! Field TSO_SPARE - SPARE register
#define TSO_SPARE_TIE0_TSO_SPARE_MASK 0xFFFFFFFFu

//! @}

//! \defgroup TSO_SPARE_TIE1 Register TSO_SPARE_TIE1
//! @{

//! Register Offset (relative)
#define TSO_SPARE_TIE1 0x10EC

//! Register Reset Value
#define TSO_SPARE_TIE1_RST 0xFFFFFFFFu

//! Field TSO_SPARE - SPARE register
#define TSO_SPARE_TIE1_TSO_SPARE_POS 0
//! Field TSO_SPARE - SPARE register
#define TSO_SPARE_TIE1_TSO_SPARE_MASK 0xFFFFFFFFu

//! @}

//! \defgroup LRO_OC_FLAG_0 Register LRO_OC_FLAG_0 - LRO Output Context Flag
//! @{

//! Register Offset (relative)
#define LRO_OC_FLAG_0 0x20A8

//! Register Reset Value
#define LRO_OC_FLAG_0_RST 0x00000000u

//! Field AGG - Aggregation flag
#define LRO_OC_FLAG_0_AGG_POS 0
//! Field AGG - Aggregation flag
#define LRO_OC_FLAG_0_AGG_MASK 0x1u
//! Constant AGG0 - single packet session
#define CONST_LRO_OC_FLAG_0_AGG_AGG0 0x0
//! Constant AGG1 - Aggregation is performance; more than one segment
#define CONST_LRO_OC_FLAG_0_AGG_AGG1 0x1

//! Field TCP_TS - TCP Timestamp Option check performed indication
#define LRO_OC_FLAG_0_TCP_TS_POS 1
//! Field TCP_TS - TCP Timestamp Option check performed indication
#define LRO_OC_FLAG_0_TCP_TS_MASK 0x2u
//! Constant NIL - not done
#define CONST_LRO_OC_FLAG_0_TCP_TS_NIL 0x0
//! Constant TS_D - Done TCP Timestamp check
#define CONST_LRO_OC_FLAG_0_TCP_TS_TS_D 0x1

//! Field EXCPT - LRO indication of any exception is processed
#define LRO_OC_FLAG_0_EXCPT_POS 2
//! Field EXCPT - LRO indication of any exception is processed
#define LRO_OC_FLAG_0_EXCPT_MASK 0x4u
//! Constant NIL - No exception
#define CONST_LRO_OC_FLAG_0_EXCPT_NIL 0x0
//! Constant EXP - any exception happened
#define CONST_LRO_OC_FLAG_0_EXCPT_EXP 0x1

//! Field EXP_CASE - Exception Reason list
#define LRO_OC_FLAG_0_EXP_CASE_POS 3
//! Field EXP_CASE - Exception Reason list
#define LRO_OC_FLAG_0_EXP_CASE_MASK 0x78u
//! Constant E0 - Flush based on GSWIP indication given
#define CONST_LRO_OC_FLAG_0_EXP_CASE_E0 0x0
//! Constant E1 - TCP check sum error
#define CONST_LRO_OC_FLAG_0_EXP_CASE_E1 0x1
//! Constant E2 - FID mismatch(not used)
#define CONST_LRO_OC_FLAG_0_EXP_CASE_E2 0x2
//! Constant E3 - TCP zero length packet
#define CONST_LRO_OC_FLAG_0_EXP_CASE_E3 0x3
//! Constant E4 - TCP Timestamp check error
#define CONST_LRO_OC_FLAG_0_EXP_CASE_E4 0x4
//! Constant E5 - Excessive length
#define CONST_LRO_OC_FLAG_0_EXP_CASE_E5 0x5
//! Constant E6 - session timeout
#define CONST_LRO_OC_FLAG_0_EXP_CASE_E6 0x6
//! Constant E7 - TCP sequence number out of order
#define CONST_LRO_OC_FLAG_0_EXP_CASE_E7 0x7

//! Field SEG_NR - Number of segments collected
#define LRO_OC_FLAG_0_SEG_NR_POS 10
//! Field SEG_NR - Number of segments collected
#define LRO_OC_FLAG_0_SEG_NR_MASK 0x7C00u

//! Field TPL - Length of all segments collected
#define LRO_OC_FLAG_0_TPL_POS 15
//! Field TPL - Length of all segments collected
#define LRO_OC_FLAG_0_TPL_MASK 0x7FFF8000u

//! @}

//! \defgroup LRO_OC_OWNER_0 Register LRO_OC_OWNER_0 - Owner bit for the output context
//! @{

//! Register Offset (relative)
#define LRO_OC_OWNER_0 0x20B0

//! Register Reset Value
#define LRO_OC_OWNER_0_RST 0x00000000u

//! Field OWNER - Owner ship of the context
#define LRO_OC_OWNER_0_OWNER_POS 31
//! Field OWNER - Owner ship of the context
#define LRO_OC_OWNER_0_OWNER_MASK 0x80000000u
//! Constant LRO - SW writes 1 to clear it to 0 .
#define CONST_LRO_OC_OWNER_0_OWNER_LRO 0x0
//! Constant SW - LRO HW writes '1' to give ownership to SW
#define CONST_LRO_OC_OWNER_0_OWNER_SW 0x1

//! @}

//! \defgroup LRO_OC_FLAG_1 Register LRO_OC_FLAG_1 - LRO Output Context Flag
//! @{

//! Register Offset (relative)
#define LRO_OC_FLAG_1 0x21A8

//! Register Reset Value
#define LRO_OC_FLAG_1_RST 0x00000000u

//! Field AGG - Aggregation flag
#define LRO_OC_FLAG_1_AGG_POS 0
//! Field AGG - Aggregation flag
#define LRO_OC_FLAG_1_AGG_MASK 0x1u
//! Constant AGG0 - single packet session
#define CONST_LRO_OC_FLAG_1_AGG_AGG0 0x0
//! Constant AGG1 - Aggregation is performance; more than one segment
#define CONST_LRO_OC_FLAG_1_AGG_AGG1 0x1

//! Field TCP_TS - TCP Timestamp Option check performed indication
#define LRO_OC_FLAG_1_TCP_TS_POS 1
//! Field TCP_TS - TCP Timestamp Option check performed indication
#define LRO_OC_FLAG_1_TCP_TS_MASK 0x2u
//! Constant NIL - not done
#define CONST_LRO_OC_FLAG_1_TCP_TS_NIL 0x0
//! Constant TS_D - Done TCP Timestamp check
#define CONST_LRO_OC_FLAG_1_TCP_TS_TS_D 0x1

//! Field EXCPT - LRO indication of any exception is processed
#define LRO_OC_FLAG_1_EXCPT_POS 2
//! Field EXCPT - LRO indication of any exception is processed
#define LRO_OC_FLAG_1_EXCPT_MASK 0x4u
//! Constant NIL - No exception
#define CONST_LRO_OC_FLAG_1_EXCPT_NIL 0x0
//! Constant EXP - any exception happened
#define CONST_LRO_OC_FLAG_1_EXCPT_EXP 0x1

//! Field EXP_CASE - Exception Reason list
#define LRO_OC_FLAG_1_EXP_CASE_POS 3
//! Field EXP_CASE - Exception Reason list
#define LRO_OC_FLAG_1_EXP_CASE_MASK 0x78u
//! Constant E0 - Flush based on GSWIP indication given
#define CONST_LRO_OC_FLAG_1_EXP_CASE_E0 0x0
//! Constant E1 - TCP check sum error
#define CONST_LRO_OC_FLAG_1_EXP_CASE_E1 0x1
//! Constant E2 - FID mismatch(not used)
#define CONST_LRO_OC_FLAG_1_EXP_CASE_E2 0x2
//! Constant E3 - TCP zero length packet
#define CONST_LRO_OC_FLAG_1_EXP_CASE_E3 0x3
//! Constant E4 - TCP Timestamp check error
#define CONST_LRO_OC_FLAG_1_EXP_CASE_E4 0x4
//! Constant E5 - Excessive length
#define CONST_LRO_OC_FLAG_1_EXP_CASE_E5 0x5
//! Constant E6 - session timeout
#define CONST_LRO_OC_FLAG_1_EXP_CASE_E6 0x6
//! Constant E7 - TCP sequence number out of order
#define CONST_LRO_OC_FLAG_1_EXP_CASE_E7 0x7

//! Field SEG_NR - Number of segments collected
#define LRO_OC_FLAG_1_SEG_NR_POS 10
//! Field SEG_NR - Number of segments collected
#define LRO_OC_FLAG_1_SEG_NR_MASK 0x7C00u

//! Field TPL - Length of all segments collected
#define LRO_OC_FLAG_1_TPL_POS 15
//! Field TPL - Length of all segments collected
#define LRO_OC_FLAG_1_TPL_MASK 0x7FFF8000u

//! @}

//! \defgroup LRO_OC_OWNER_1 Register LRO_OC_OWNER_1 - Owner bit for the output context
//! @{

//! Register Offset (relative)
#define LRO_OC_OWNER_1 0x21B0

//! Register Reset Value
#define LRO_OC_OWNER_1_RST 0x00000000u

//! Field OWNER - Owner ship of the context
#define LRO_OC_OWNER_1_OWNER_POS 31
//! Field OWNER - Owner ship of the context
#define LRO_OC_OWNER_1_OWNER_MASK 0x80000000u
//! Constant LRO - SW writes 1 to clear it to 0 .
#define CONST_LRO_OC_OWNER_1_OWNER_LRO 0x0
//! Constant SW - LRO HW writes '1' to give ownership to SW
#define CONST_LRO_OC_OWNER_1_OWNER_SW 0x1

//! @}

//! \defgroup LRO_OC_FLAG_2 Register LRO_OC_FLAG_2 - LRO Output Context Flag
//! @{

//! Register Offset (relative)
#define LRO_OC_FLAG_2 0x22A8

//! Register Reset Value
#define LRO_OC_FLAG_2_RST 0x00000000u

//! Field AGG - Aggregation flag
#define LRO_OC_FLAG_2_AGG_POS 0
//! Field AGG - Aggregation flag
#define LRO_OC_FLAG_2_AGG_MASK 0x1u
//! Constant AGG0 - single packet session
#define CONST_LRO_OC_FLAG_2_AGG_AGG0 0x0
//! Constant AGG1 - Aggregation is performance; more than one segment
#define CONST_LRO_OC_FLAG_2_AGG_AGG1 0x1

//! Field TCP_TS - TCP Timestamp Option check performed indication
#define LRO_OC_FLAG_2_TCP_TS_POS 1
//! Field TCP_TS - TCP Timestamp Option check performed indication
#define LRO_OC_FLAG_2_TCP_TS_MASK 0x2u
//! Constant NIL - not done
#define CONST_LRO_OC_FLAG_2_TCP_TS_NIL 0x0
//! Constant TS_D - Done TCP Timestamp check
#define CONST_LRO_OC_FLAG_2_TCP_TS_TS_D 0x1

//! Field EXCPT - LRO indication of any exception is processed
#define LRO_OC_FLAG_2_EXCPT_POS 2
//! Field EXCPT - LRO indication of any exception is processed
#define LRO_OC_FLAG_2_EXCPT_MASK 0x4u
//! Constant NIL - No exception
#define CONST_LRO_OC_FLAG_2_EXCPT_NIL 0x0
//! Constant EXP - any exception happened
#define CONST_LRO_OC_FLAG_2_EXCPT_EXP 0x1

//! Field EXP_CASE - Exception Reason list
#define LRO_OC_FLAG_2_EXP_CASE_POS 3
//! Field EXP_CASE - Exception Reason list
#define LRO_OC_FLAG_2_EXP_CASE_MASK 0x78u
//! Constant E0 - Flush based on GSWIP indication given
#define CONST_LRO_OC_FLAG_2_EXP_CASE_E0 0x0
//! Constant E1 - TCP check sum error
#define CONST_LRO_OC_FLAG_2_EXP_CASE_E1 0x1
//! Constant E2 - FID mismatch(not used)
#define CONST_LRO_OC_FLAG_2_EXP_CASE_E2 0x2
//! Constant E3 - TCP zero length packet
#define CONST_LRO_OC_FLAG_2_EXP_CASE_E3 0x3
//! Constant E4 - TCP Timestamp check error
#define CONST_LRO_OC_FLAG_2_EXP_CASE_E4 0x4
//! Constant E5 - Excessive length
#define CONST_LRO_OC_FLAG_2_EXP_CASE_E5 0x5
//! Constant E6 - session timeout
#define CONST_LRO_OC_FLAG_2_EXP_CASE_E6 0x6
//! Constant E7 - TCP sequence number out of order
#define CONST_LRO_OC_FLAG_2_EXP_CASE_E7 0x7

//! Field SEG_NR - Number of segments collected
#define LRO_OC_FLAG_2_SEG_NR_POS 10
//! Field SEG_NR - Number of segments collected
#define LRO_OC_FLAG_2_SEG_NR_MASK 0x7C00u

//! Field TPL - Length of all segments collected
#define LRO_OC_FLAG_2_TPL_POS 15
//! Field TPL - Length of all segments collected
#define LRO_OC_FLAG_2_TPL_MASK 0x7FFF8000u

//! @}

//! \defgroup LRO_OC_OWNER_2 Register LRO_OC_OWNER_2 - Owner bit for the output context
//! @{

//! Register Offset (relative)
#define LRO_OC_OWNER_2 0x22B0

//! Register Reset Value
#define LRO_OC_OWNER_2_RST 0x00000000u

//! Field OWNER - Owner ship of the context
#define LRO_OC_OWNER_2_OWNER_POS 31
//! Field OWNER - Owner ship of the context
#define LRO_OC_OWNER_2_OWNER_MASK 0x80000000u
//! Constant LRO - SW writes 1 to clear it to 0 .
#define CONST_LRO_OC_OWNER_2_OWNER_LRO 0x0
//! Constant SW - LRO HW writes '1' to give ownership to SW
#define CONST_LRO_OC_OWNER_2_OWNER_SW 0x1

//! @}

//! \defgroup LRO_OC_FLAG_3 Register LRO_OC_FLAG_3 - LRO Output Context Flag
//! @{

//! Register Offset (relative)
#define LRO_OC_FLAG_3 0x23A8

//! Register Reset Value
#define LRO_OC_FLAG_3_RST 0x00000000u

//! Field AGG - Aggregation flag
#define LRO_OC_FLAG_3_AGG_POS 0
//! Field AGG - Aggregation flag
#define LRO_OC_FLAG_3_AGG_MASK 0x1u
//! Constant AGG0 - single packet session
#define CONST_LRO_OC_FLAG_3_AGG_AGG0 0x0
//! Constant AGG1 - Aggregation is performance; more than one segment
#define CONST_LRO_OC_FLAG_3_AGG_AGG1 0x1

//! Field TCP_TS - TCP Timestamp Option check performed indication
#define LRO_OC_FLAG_3_TCP_TS_POS 1
//! Field TCP_TS - TCP Timestamp Option check performed indication
#define LRO_OC_FLAG_3_TCP_TS_MASK 0x2u
//! Constant NIL - not done
#define CONST_LRO_OC_FLAG_3_TCP_TS_NIL 0x0
//! Constant TS_D - Done TCP Timestamp check
#define CONST_LRO_OC_FLAG_3_TCP_TS_TS_D 0x1

//! Field EXCPT - LRO indication of any exception is processed
#define LRO_OC_FLAG_3_EXCPT_POS 2
//! Field EXCPT - LRO indication of any exception is processed
#define LRO_OC_FLAG_3_EXCPT_MASK 0x4u
//! Constant NIL - No exception
#define CONST_LRO_OC_FLAG_3_EXCPT_NIL 0x0
//! Constant EXP - any exception happened
#define CONST_LRO_OC_FLAG_3_EXCPT_EXP 0x1

//! Field EXP_CASE - Exception Reason list
#define LRO_OC_FLAG_3_EXP_CASE_POS 3
//! Field EXP_CASE - Exception Reason list
#define LRO_OC_FLAG_3_EXP_CASE_MASK 0x78u
//! Constant E0 - Flush based on GSWIP indication given
#define CONST_LRO_OC_FLAG_3_EXP_CASE_E0 0x0
//! Constant E1 - TCP check sum error
#define CONST_LRO_OC_FLAG_3_EXP_CASE_E1 0x1
//! Constant E2 - FID mismatch(not used)
#define CONST_LRO_OC_FLAG_3_EXP_CASE_E2 0x2
//! Constant E3 - TCP zero length packet
#define CONST_LRO_OC_FLAG_3_EXP_CASE_E3 0x3
//! Constant E4 - TCP Timestamp check error
#define CONST_LRO_OC_FLAG_3_EXP_CASE_E4 0x4
//! Constant E5 - Excessive length
#define CONST_LRO_OC_FLAG_3_EXP_CASE_E5 0x5
//! Constant E6 - session timeout
#define CONST_LRO_OC_FLAG_3_EXP_CASE_E6 0x6
//! Constant E7 - TCP sequence number out of order
#define CONST_LRO_OC_FLAG_3_EXP_CASE_E7 0x7

//! Field SEG_NR - Number of segments collected
#define LRO_OC_FLAG_3_SEG_NR_POS 10
//! Field SEG_NR - Number of segments collected
#define LRO_OC_FLAG_3_SEG_NR_MASK 0x7C00u

//! Field TPL - Length of all segments collected
#define LRO_OC_FLAG_3_TPL_POS 15
//! Field TPL - Length of all segments collected
#define LRO_OC_FLAG_3_TPL_MASK 0x7FFF8000u

//! @}

//! \defgroup LRO_OC_OWNER_3 Register LRO_OC_OWNER_3 - Owner bit for the output context
//! @{

//! Register Offset (relative)
#define LRO_OC_OWNER_3 0x23B0

//! Register Reset Value
#define LRO_OC_OWNER_3_RST 0x00000000u

//! Field OWNER - Owner ship of the context
#define LRO_OC_OWNER_3_OWNER_POS 31
//! Field OWNER - Owner ship of the context
#define LRO_OC_OWNER_3_OWNER_MASK 0x80000000u
//! Constant LRO - SW writes 1 to clear it to 0 .
#define CONST_LRO_OC_OWNER_3_OWNER_LRO 0x0
//! Constant SW - LRO HW writes '1' to give ownership to SW
#define CONST_LRO_OC_OWNER_3_OWNER_SW 0x1

//! @}

//! \defgroup LRO_OC_FLAG_4 Register LRO_OC_FLAG_4 - LRO Output Context Flag
//! @{

//! Register Offset (relative)
#define LRO_OC_FLAG_4 0x24A8

//! Register Reset Value
#define LRO_OC_FLAG_4_RST 0x00000000u

//! Field AGG - Aggregation flag
#define LRO_OC_FLAG_4_AGG_POS 0
//! Field AGG - Aggregation flag
#define LRO_OC_FLAG_4_AGG_MASK 0x1u
//! Constant AGG0 - single packet session
#define CONST_LRO_OC_FLAG_4_AGG_AGG0 0x0
//! Constant AGG1 - Aggregation is performance; more than one segment
#define CONST_LRO_OC_FLAG_4_AGG_AGG1 0x1

//! Field TCP_TS - TCP Timestamp Option check performed indication
#define LRO_OC_FLAG_4_TCP_TS_POS 1
//! Field TCP_TS - TCP Timestamp Option check performed indication
#define LRO_OC_FLAG_4_TCP_TS_MASK 0x2u
//! Constant NIL - not done
#define CONST_LRO_OC_FLAG_4_TCP_TS_NIL 0x0
//! Constant TS_D - Done TCP Timestamp check
#define CONST_LRO_OC_FLAG_4_TCP_TS_TS_D 0x1

//! Field EXCPT - LRO indication of any exception is processed
#define LRO_OC_FLAG_4_EXCPT_POS 2
//! Field EXCPT - LRO indication of any exception is processed
#define LRO_OC_FLAG_4_EXCPT_MASK 0x4u
//! Constant NIL - No exception
#define CONST_LRO_OC_FLAG_4_EXCPT_NIL 0x0
//! Constant EXP - any exception happened
#define CONST_LRO_OC_FLAG_4_EXCPT_EXP 0x1

//! Field EXP_CASE - Exception Reason list
#define LRO_OC_FLAG_4_EXP_CASE_POS 3
//! Field EXP_CASE - Exception Reason list
#define LRO_OC_FLAG_4_EXP_CASE_MASK 0x78u
//! Constant E0 - Flush based on GSWIP indication given
#define CONST_LRO_OC_FLAG_4_EXP_CASE_E0 0x0
//! Constant E1 - TCP check sum error
#define CONST_LRO_OC_FLAG_4_EXP_CASE_E1 0x1
//! Constant E2 - FID mismatch(not used)
#define CONST_LRO_OC_FLAG_4_EXP_CASE_E2 0x2
//! Constant E3 - TCP zero length packet
#define CONST_LRO_OC_FLAG_4_EXP_CASE_E3 0x3
//! Constant E4 - TCP Timestamp check error
#define CONST_LRO_OC_FLAG_4_EXP_CASE_E4 0x4
//! Constant E5 - Excessive length
#define CONST_LRO_OC_FLAG_4_EXP_CASE_E5 0x5
//! Constant E6 - session timeout
#define CONST_LRO_OC_FLAG_4_EXP_CASE_E6 0x6
//! Constant E7 - TCP sequence number out of order
#define CONST_LRO_OC_FLAG_4_EXP_CASE_E7 0x7

//! Field SEG_NR - Number of segments collected
#define LRO_OC_FLAG_4_SEG_NR_POS 10
//! Field SEG_NR - Number of segments collected
#define LRO_OC_FLAG_4_SEG_NR_MASK 0x7C00u

//! Field TPL - Length of all segments collected
#define LRO_OC_FLAG_4_TPL_POS 15
//! Field TPL - Length of all segments collected
#define LRO_OC_FLAG_4_TPL_MASK 0x7FFF8000u

//! @}

//! \defgroup LRO_OC_OWNER_4 Register LRO_OC_OWNER_4 - Owner bit for the output context
//! @{

//! Register Offset (relative)
#define LRO_OC_OWNER_4 0x24B0

//! Register Reset Value
#define LRO_OC_OWNER_4_RST 0x00000000u

//! Field OWNER - Owner ship of the context
#define LRO_OC_OWNER_4_OWNER_POS 31
//! Field OWNER - Owner ship of the context
#define LRO_OC_OWNER_4_OWNER_MASK 0x80000000u
//! Constant LRO - SW writes 1 to clear it to 0 .
#define CONST_LRO_OC_OWNER_4_OWNER_LRO 0x0
//! Constant SW - LRO HW writes '1' to give ownership to SW
#define CONST_LRO_OC_OWNER_4_OWNER_SW 0x1

//! @}

//! \defgroup LRO_OC_FLAG_5 Register LRO_OC_FLAG_5 - LRO Output Context Flag
//! @{

//! Register Offset (relative)
#define LRO_OC_FLAG_5 0x25A8

//! Register Reset Value
#define LRO_OC_FLAG_5_RST 0x00000000u

//! Field AGG - Aggregation flag
#define LRO_OC_FLAG_5_AGG_POS 0
//! Field AGG - Aggregation flag
#define LRO_OC_FLAG_5_AGG_MASK 0x1u
//! Constant AGG0 - single packet session
#define CONST_LRO_OC_FLAG_5_AGG_AGG0 0x0
//! Constant AGG1 - Aggregation is performance; more than one segment
#define CONST_LRO_OC_FLAG_5_AGG_AGG1 0x1

//! Field TCP_TS - TCP Timestamp Option check performed indication
#define LRO_OC_FLAG_5_TCP_TS_POS 1
//! Field TCP_TS - TCP Timestamp Option check performed indication
#define LRO_OC_FLAG_5_TCP_TS_MASK 0x2u
//! Constant NIL - not done
#define CONST_LRO_OC_FLAG_5_TCP_TS_NIL 0x0
//! Constant TS_D - Done TCP Timestamp check
#define CONST_LRO_OC_FLAG_5_TCP_TS_TS_D 0x1

//! Field EXCPT - LRO indication of any exception is processed
#define LRO_OC_FLAG_5_EXCPT_POS 2
//! Field EXCPT - LRO indication of any exception is processed
#define LRO_OC_FLAG_5_EXCPT_MASK 0x4u
//! Constant NIL - No exception
#define CONST_LRO_OC_FLAG_5_EXCPT_NIL 0x0
//! Constant EXP - any exception happened
#define CONST_LRO_OC_FLAG_5_EXCPT_EXP 0x1

//! Field EXP_CASE - Exception Reason list
#define LRO_OC_FLAG_5_EXP_CASE_POS 3
//! Field EXP_CASE - Exception Reason list
#define LRO_OC_FLAG_5_EXP_CASE_MASK 0x78u
//! Constant E0 - Flush based on GSWIP indication given
#define CONST_LRO_OC_FLAG_5_EXP_CASE_E0 0x0
//! Constant E1 - TCP check sum error
#define CONST_LRO_OC_FLAG_5_EXP_CASE_E1 0x1
//! Constant E2 - FID mismatch(not used)
#define CONST_LRO_OC_FLAG_5_EXP_CASE_E2 0x2
//! Constant E3 - TCP zero length packet
#define CONST_LRO_OC_FLAG_5_EXP_CASE_E3 0x3
//! Constant E4 - TCP Timestamp check error
#define CONST_LRO_OC_FLAG_5_EXP_CASE_E4 0x4
//! Constant E5 - Excessive length
#define CONST_LRO_OC_FLAG_5_EXP_CASE_E5 0x5
//! Constant E6 - session timeout
#define CONST_LRO_OC_FLAG_5_EXP_CASE_E6 0x6
//! Constant E7 - TCP sequence number out of order
#define CONST_LRO_OC_FLAG_5_EXP_CASE_E7 0x7

//! Field SEG_NR - Number of segments collected
#define LRO_OC_FLAG_5_SEG_NR_POS 10
//! Field SEG_NR - Number of segments collected
#define LRO_OC_FLAG_5_SEG_NR_MASK 0x7C00u

//! Field TPL - Length of all segments collected
#define LRO_OC_FLAG_5_TPL_POS 15
//! Field TPL - Length of all segments collected
#define LRO_OC_FLAG_5_TPL_MASK 0x7FFF8000u

//! @}

//! \defgroup LRO_OC_OWNER_5 Register LRO_OC_OWNER_5 - Owner bit for the output context
//! @{

//! Register Offset (relative)
#define LRO_OC_OWNER_5 0x25B0

//! Register Reset Value
#define LRO_OC_OWNER_5_RST 0x00000000u

//! Field OWNER - Owner ship of the context
#define LRO_OC_OWNER_5_OWNER_POS 31
//! Field OWNER - Owner ship of the context
#define LRO_OC_OWNER_5_OWNER_MASK 0x80000000u
//! Constant LRO - SW writes 1 to clear it to 0 .
#define CONST_LRO_OC_OWNER_5_OWNER_LRO 0x0
//! Constant SW - LRO HW writes '1' to give ownership to SW
#define CONST_LRO_OC_OWNER_5_OWNER_SW 0x1

//! @}

//! \defgroup LRO_OC_FLAG_6 Register LRO_OC_FLAG_6 - LRO Output Context Flag
//! @{

//! Register Offset (relative)
#define LRO_OC_FLAG_6 0x26A8

//! Register Reset Value
#define LRO_OC_FLAG_6_RST 0x00000000u

//! Field AGG - Aggregation flag
#define LRO_OC_FLAG_6_AGG_POS 0
//! Field AGG - Aggregation flag
#define LRO_OC_FLAG_6_AGG_MASK 0x1u
//! Constant AGG0 - single packet session
#define CONST_LRO_OC_FLAG_6_AGG_AGG0 0x0
//! Constant AGG1 - Aggregation is performance; more than one segment
#define CONST_LRO_OC_FLAG_6_AGG_AGG1 0x1

//! Field TCP_TS - TCP Timestamp Option check performed indication
#define LRO_OC_FLAG_6_TCP_TS_POS 1
//! Field TCP_TS - TCP Timestamp Option check performed indication
#define LRO_OC_FLAG_6_TCP_TS_MASK 0x2u
//! Constant NIL - not done
#define CONST_LRO_OC_FLAG_6_TCP_TS_NIL 0x0
//! Constant TS_D - Done TCP Timestamp check
#define CONST_LRO_OC_FLAG_6_TCP_TS_TS_D 0x1

//! Field EXCPT - LRO indication of any exception is processed
#define LRO_OC_FLAG_6_EXCPT_POS 2
//! Field EXCPT - LRO indication of any exception is processed
#define LRO_OC_FLAG_6_EXCPT_MASK 0x4u
//! Constant NIL - No exception
#define CONST_LRO_OC_FLAG_6_EXCPT_NIL 0x0
//! Constant EXP - any exception happened
#define CONST_LRO_OC_FLAG_6_EXCPT_EXP 0x1

//! Field EXP_CASE - Exception Reason list
#define LRO_OC_FLAG_6_EXP_CASE_POS 3
//! Field EXP_CASE - Exception Reason list
#define LRO_OC_FLAG_6_EXP_CASE_MASK 0x78u
//! Constant E0 - Flush based on GSWIP indication given
#define CONST_LRO_OC_FLAG_6_EXP_CASE_E0 0x0
//! Constant E1 - TCP check sum error
#define CONST_LRO_OC_FLAG_6_EXP_CASE_E1 0x1
//! Constant E2 - FID mismatch(not used)
#define CONST_LRO_OC_FLAG_6_EXP_CASE_E2 0x2
//! Constant E3 - TCP zero length packet
#define CONST_LRO_OC_FLAG_6_EXP_CASE_E3 0x3
//! Constant E4 - TCP Timestamp check error
#define CONST_LRO_OC_FLAG_6_EXP_CASE_E4 0x4
//! Constant E5 - Excessive length
#define CONST_LRO_OC_FLAG_6_EXP_CASE_E5 0x5
//! Constant E6 - session timeout
#define CONST_LRO_OC_FLAG_6_EXP_CASE_E6 0x6
//! Constant E7 - TCP sequence number out of order
#define CONST_LRO_OC_FLAG_6_EXP_CASE_E7 0x7

//! Field SEG_NR - Number of segments collected
#define LRO_OC_FLAG_6_SEG_NR_POS 10
//! Field SEG_NR - Number of segments collected
#define LRO_OC_FLAG_6_SEG_NR_MASK 0x7C00u

//! Field TPL - Length of all segments collected
#define LRO_OC_FLAG_6_TPL_POS 15
//! Field TPL - Length of all segments collected
#define LRO_OC_FLAG_6_TPL_MASK 0x7FFF8000u

//! @}

//! \defgroup LRO_OC_OWNER_6 Register LRO_OC_OWNER_6 - Owner bit for the output context
//! @{

//! Register Offset (relative)
#define LRO_OC_OWNER_6 0x26B0

//! Register Reset Value
#define LRO_OC_OWNER_6_RST 0x00000000u

//! Field OWNER - Owner ship of the context
#define LRO_OC_OWNER_6_OWNER_POS 31
//! Field OWNER - Owner ship of the context
#define LRO_OC_OWNER_6_OWNER_MASK 0x80000000u
//! Constant LRO - SW writes 1 to clear it to 0 .
#define CONST_LRO_OC_OWNER_6_OWNER_LRO 0x0
//! Constant SW - LRO HW writes '1' to give ownership to SW
#define CONST_LRO_OC_OWNER_6_OWNER_SW 0x1

//! @}

//! \defgroup LRO_OC_FLAG_7 Register LRO_OC_FLAG_7 - LRO Output Context Flag
//! @{

//! Register Offset (relative)
#define LRO_OC_FLAG_7 0x27A8

//! Register Reset Value
#define LRO_OC_FLAG_7_RST 0x00000000u

//! Field AGG - Aggregation flag
#define LRO_OC_FLAG_7_AGG_POS 0
//! Field AGG - Aggregation flag
#define LRO_OC_FLAG_7_AGG_MASK 0x1u
//! Constant AGG0 - single packet session
#define CONST_LRO_OC_FLAG_7_AGG_AGG0 0x0
//! Constant AGG1 - Aggregation is performance; more than one segment
#define CONST_LRO_OC_FLAG_7_AGG_AGG1 0x1

//! Field TCP_TS - TCP Timestamp Option check performed indication
#define LRO_OC_FLAG_7_TCP_TS_POS 1
//! Field TCP_TS - TCP Timestamp Option check performed indication
#define LRO_OC_FLAG_7_TCP_TS_MASK 0x2u
//! Constant NIL - not done
#define CONST_LRO_OC_FLAG_7_TCP_TS_NIL 0x0
//! Constant TS_D - Done TCP Timestamp check
#define CONST_LRO_OC_FLAG_7_TCP_TS_TS_D 0x1

//! Field EXCPT - LRO indication of any exception is processed
#define LRO_OC_FLAG_7_EXCPT_POS 2
//! Field EXCPT - LRO indication of any exception is processed
#define LRO_OC_FLAG_7_EXCPT_MASK 0x4u
//! Constant NIL - No exception
#define CONST_LRO_OC_FLAG_7_EXCPT_NIL 0x0
//! Constant EXP - any exception happened
#define CONST_LRO_OC_FLAG_7_EXCPT_EXP 0x1

//! Field EXP_CASE - Exception Reason list
#define LRO_OC_FLAG_7_EXP_CASE_POS 3
//! Field EXP_CASE - Exception Reason list
#define LRO_OC_FLAG_7_EXP_CASE_MASK 0x78u
//! Constant E0 - Flush based on GSWIP indication given
#define CONST_LRO_OC_FLAG_7_EXP_CASE_E0 0x0
//! Constant E1 - TCP check sum error
#define CONST_LRO_OC_FLAG_7_EXP_CASE_E1 0x1
//! Constant E2 - FID mismatch(not used)
#define CONST_LRO_OC_FLAG_7_EXP_CASE_E2 0x2
//! Constant E3 - TCP zero length packet
#define CONST_LRO_OC_FLAG_7_EXP_CASE_E3 0x3
//! Constant E4 - TCP Timestamp check error
#define CONST_LRO_OC_FLAG_7_EXP_CASE_E4 0x4
//! Constant E5 - Excessive length
#define CONST_LRO_OC_FLAG_7_EXP_CASE_E5 0x5
//! Constant E6 - session timeout
#define CONST_LRO_OC_FLAG_7_EXP_CASE_E6 0x6
//! Constant E7 - TCP sequence number out of order
#define CONST_LRO_OC_FLAG_7_EXP_CASE_E7 0x7

//! Field SEG_NR - Number of segments collected
#define LRO_OC_FLAG_7_SEG_NR_POS 10
//! Field SEG_NR - Number of segments collected
#define LRO_OC_FLAG_7_SEG_NR_MASK 0x7C00u

//! Field TPL - Length of all segments collected
#define LRO_OC_FLAG_7_TPL_POS 15
//! Field TPL - Length of all segments collected
#define LRO_OC_FLAG_7_TPL_MASK 0x7FFF8000u

//! @}

//! \defgroup LRO_OC_OWNER_7 Register LRO_OC_OWNER_7 - Owner bit for the output context
//! @{

//! Register Offset (relative)
#define LRO_OC_OWNER_7 0x27B0

//! Register Reset Value
#define LRO_OC_OWNER_7_RST 0x00000000u

//! Field OWNER - Owner ship of the context
#define LRO_OC_OWNER_7_OWNER_POS 31
//! Field OWNER - Owner ship of the context
#define LRO_OC_OWNER_7_OWNER_MASK 0x80000000u
//! Constant LRO - SW writes 1 to clear it to 0 .
#define CONST_LRO_OC_OWNER_7_OWNER_LRO 0x0
//! Constant SW - LRO HW writes '1' to give ownership to SW
#define CONST_LRO_OC_OWNER_7_OWNER_SW 0x1

//! @}

//! \defgroup LRO_OC_FLAG_8 Register LRO_OC_FLAG_8 - LRO Output Context Flag
//! @{

//! Register Offset (relative)
#define LRO_OC_FLAG_8 0x28A8

//! Register Reset Value
#define LRO_OC_FLAG_8_RST 0x00000000u

//! Field AGG - Aggregation flag
#define LRO_OC_FLAG_8_AGG_POS 0
//! Field AGG - Aggregation flag
#define LRO_OC_FLAG_8_AGG_MASK 0x1u
//! Constant AGG0 - single packet session
#define CONST_LRO_OC_FLAG_8_AGG_AGG0 0x0
//! Constant AGG1 - Aggregation is performance; more than one segment
#define CONST_LRO_OC_FLAG_8_AGG_AGG1 0x1

//! Field TCP_TS - TCP Timestamp Option check performed indication
#define LRO_OC_FLAG_8_TCP_TS_POS 1
//! Field TCP_TS - TCP Timestamp Option check performed indication
#define LRO_OC_FLAG_8_TCP_TS_MASK 0x2u
//! Constant NIL - not done
#define CONST_LRO_OC_FLAG_8_TCP_TS_NIL 0x0
//! Constant TS_D - Done TCP Timestamp check
#define CONST_LRO_OC_FLAG_8_TCP_TS_TS_D 0x1

//! Field EXCPT - LRO indication of any exception is processed
#define LRO_OC_FLAG_8_EXCPT_POS 2
//! Field EXCPT - LRO indication of any exception is processed
#define LRO_OC_FLAG_8_EXCPT_MASK 0x4u
//! Constant NIL - No exception
#define CONST_LRO_OC_FLAG_8_EXCPT_NIL 0x0
//! Constant EXP - any exception happened
#define CONST_LRO_OC_FLAG_8_EXCPT_EXP 0x1

//! Field EXP_CASE - Exception Reason list
#define LRO_OC_FLAG_8_EXP_CASE_POS 3
//! Field EXP_CASE - Exception Reason list
#define LRO_OC_FLAG_8_EXP_CASE_MASK 0x78u
//! Constant E0 - Flush based on GSWIP indication given
#define CONST_LRO_OC_FLAG_8_EXP_CASE_E0 0x0
//! Constant E1 - TCP check sum error
#define CONST_LRO_OC_FLAG_8_EXP_CASE_E1 0x1
//! Constant E2 - FID mismatch(not used)
#define CONST_LRO_OC_FLAG_8_EXP_CASE_E2 0x2
//! Constant E3 - TCP zero length packet
#define CONST_LRO_OC_FLAG_8_EXP_CASE_E3 0x3
//! Constant E4 - TCP Timestamp check error
#define CONST_LRO_OC_FLAG_8_EXP_CASE_E4 0x4
//! Constant E5 - Excessive length
#define CONST_LRO_OC_FLAG_8_EXP_CASE_E5 0x5
//! Constant E6 - session timeout
#define CONST_LRO_OC_FLAG_8_EXP_CASE_E6 0x6
//! Constant E7 - TCP sequence number out of order
#define CONST_LRO_OC_FLAG_8_EXP_CASE_E7 0x7

//! Field SEG_NR - Number of segments collected
#define LRO_OC_FLAG_8_SEG_NR_POS 10
//! Field SEG_NR - Number of segments collected
#define LRO_OC_FLAG_8_SEG_NR_MASK 0x7C00u

//! Field TPL - Length of all segments collected
#define LRO_OC_FLAG_8_TPL_POS 15
//! Field TPL - Length of all segments collected
#define LRO_OC_FLAG_8_TPL_MASK 0x7FFF8000u

//! @}

//! \defgroup LRO_OC_OWNER_8 Register LRO_OC_OWNER_8 - Owner bit for the output context
//! @{

//! Register Offset (relative)
#define LRO_OC_OWNER_8 0x28B0

//! Register Reset Value
#define LRO_OC_OWNER_8_RST 0x00000000u

//! Field OWNER - Owner ship of the context
#define LRO_OC_OWNER_8_OWNER_POS 31
//! Field OWNER - Owner ship of the context
#define LRO_OC_OWNER_8_OWNER_MASK 0x80000000u
//! Constant LRO - SW writes 1 to clear it to 0 .
#define CONST_LRO_OC_OWNER_8_OWNER_LRO 0x0
//! Constant SW - LRO HW writes '1' to give ownership to SW
#define CONST_LRO_OC_OWNER_8_OWNER_SW 0x1

//! @}

//! \defgroup LRO_OC_FLAG_9 Register LRO_OC_FLAG_9 - LRO Output Context Flag
//! @{

//! Register Offset (relative)
#define LRO_OC_FLAG_9 0x29A8

//! Register Reset Value
#define LRO_OC_FLAG_9_RST 0x00000000u

//! Field AGG - Aggregation flag
#define LRO_OC_FLAG_9_AGG_POS 0
//! Field AGG - Aggregation flag
#define LRO_OC_FLAG_9_AGG_MASK 0x1u
//! Constant AGG0 - single packet session
#define CONST_LRO_OC_FLAG_9_AGG_AGG0 0x0
//! Constant AGG1 - Aggregation is performance; more than one segment
#define CONST_LRO_OC_FLAG_9_AGG_AGG1 0x1

//! Field TCP_TS - TCP Timestamp Option check performed indication
#define LRO_OC_FLAG_9_TCP_TS_POS 1
//! Field TCP_TS - TCP Timestamp Option check performed indication
#define LRO_OC_FLAG_9_TCP_TS_MASK 0x2u
//! Constant NIL - not done
#define CONST_LRO_OC_FLAG_9_TCP_TS_NIL 0x0
//! Constant TS_D - Done TCP Timestamp check
#define CONST_LRO_OC_FLAG_9_TCP_TS_TS_D 0x1

//! Field EXCPT - LRO indication of any exception is processed
#define LRO_OC_FLAG_9_EXCPT_POS 2
//! Field EXCPT - LRO indication of any exception is processed
#define LRO_OC_FLAG_9_EXCPT_MASK 0x4u
//! Constant NIL - No exception
#define CONST_LRO_OC_FLAG_9_EXCPT_NIL 0x0
//! Constant EXP - any exception happened
#define CONST_LRO_OC_FLAG_9_EXCPT_EXP 0x1

//! Field EXP_CASE - Exception Reason list
#define LRO_OC_FLAG_9_EXP_CASE_POS 3
//! Field EXP_CASE - Exception Reason list
#define LRO_OC_FLAG_9_EXP_CASE_MASK 0x78u
//! Constant E0 - Flush based on GSWIP indication given
#define CONST_LRO_OC_FLAG_9_EXP_CASE_E0 0x0
//! Constant E1 - TCP check sum error
#define CONST_LRO_OC_FLAG_9_EXP_CASE_E1 0x1
//! Constant E2 - FID mismatch(not used)
#define CONST_LRO_OC_FLAG_9_EXP_CASE_E2 0x2
//! Constant E3 - TCP zero length packet
#define CONST_LRO_OC_FLAG_9_EXP_CASE_E3 0x3
//! Constant E4 - TCP Timestamp check error
#define CONST_LRO_OC_FLAG_9_EXP_CASE_E4 0x4
//! Constant E5 - Excessive length
#define CONST_LRO_OC_FLAG_9_EXP_CASE_E5 0x5
//! Constant E6 - session timeout
#define CONST_LRO_OC_FLAG_9_EXP_CASE_E6 0x6
//! Constant E7 - TCP sequence number out of order
#define CONST_LRO_OC_FLAG_9_EXP_CASE_E7 0x7

//! Field SEG_NR - Number of segments collected
#define LRO_OC_FLAG_9_SEG_NR_POS 10
//! Field SEG_NR - Number of segments collected
#define LRO_OC_FLAG_9_SEG_NR_MASK 0x7C00u

//! Field TPL - Length of all segments collected
#define LRO_OC_FLAG_9_TPL_POS 15
//! Field TPL - Length of all segments collected
#define LRO_OC_FLAG_9_TPL_MASK 0x7FFF8000u

//! @}

//! \defgroup LRO_OC_OWNER_9 Register LRO_OC_OWNER_9 - Owner bit for the output context
//! @{

//! Register Offset (relative)
#define LRO_OC_OWNER_9 0x29B0

//! Register Reset Value
#define LRO_OC_OWNER_9_RST 0x00000000u

//! Field OWNER - Owner ship of the context
#define LRO_OC_OWNER_9_OWNER_POS 31
//! Field OWNER - Owner ship of the context
#define LRO_OC_OWNER_9_OWNER_MASK 0x80000000u
//! Constant LRO - SW writes 1 to clear it to 0 .
#define CONST_LRO_OC_OWNER_9_OWNER_LRO 0x0
//! Constant SW - LRO HW writes '1' to give ownership to SW
#define CONST_LRO_OC_OWNER_9_OWNER_SW 0x1

//! @}

//! \defgroup LRO_OC_FLAG_10 Register LRO_OC_FLAG_10 - LRO Output Context Flag
//! @{

//! Register Offset (relative)
#define LRO_OC_FLAG_10 0x2AA8

//! Register Reset Value
#define LRO_OC_FLAG_10_RST 0x00000000u

//! Field AGG - Aggregation flag
#define LRO_OC_FLAG_10_AGG_POS 0
//! Field AGG - Aggregation flag
#define LRO_OC_FLAG_10_AGG_MASK 0x1u
//! Constant AGG0 - single packet session
#define CONST_LRO_OC_FLAG_10_AGG_AGG0 0x0
//! Constant AGG1 - Aggregation is performance; more than one segment
#define CONST_LRO_OC_FLAG_10_AGG_AGG1 0x1

//! Field TCP_TS - TCP Timestamp Option check performed indication
#define LRO_OC_FLAG_10_TCP_TS_POS 1
//! Field TCP_TS - TCP Timestamp Option check performed indication
#define LRO_OC_FLAG_10_TCP_TS_MASK 0x2u
//! Constant NIL - not done
#define CONST_LRO_OC_FLAG_10_TCP_TS_NIL 0x0
//! Constant TS_D - Done TCP Timestamp check
#define CONST_LRO_OC_FLAG_10_TCP_TS_TS_D 0x1

//! Field EXCPT - LRO indication of any exception is processed
#define LRO_OC_FLAG_10_EXCPT_POS 2
//! Field EXCPT - LRO indication of any exception is processed
#define LRO_OC_FLAG_10_EXCPT_MASK 0x4u
//! Constant NIL - No exception
#define CONST_LRO_OC_FLAG_10_EXCPT_NIL 0x0
//! Constant EXP - any exception happened
#define CONST_LRO_OC_FLAG_10_EXCPT_EXP 0x1

//! Field EXP_CASE - Exception Reason list
#define LRO_OC_FLAG_10_EXP_CASE_POS 3
//! Field EXP_CASE - Exception Reason list
#define LRO_OC_FLAG_10_EXP_CASE_MASK 0x78u
//! Constant E0 - Flush based on GSWIP indication given
#define CONST_LRO_OC_FLAG_10_EXP_CASE_E0 0x0
//! Constant E1 - TCP check sum error
#define CONST_LRO_OC_FLAG_10_EXP_CASE_E1 0x1
//! Constant E2 - FID mismatch(not used)
#define CONST_LRO_OC_FLAG_10_EXP_CASE_E2 0x2
//! Constant E3 - TCP zero length packet
#define CONST_LRO_OC_FLAG_10_EXP_CASE_E3 0x3
//! Constant E4 - TCP Timestamp check error
#define CONST_LRO_OC_FLAG_10_EXP_CASE_E4 0x4
//! Constant E5 - Excessive length
#define CONST_LRO_OC_FLAG_10_EXP_CASE_E5 0x5
//! Constant E6 - session timeout
#define CONST_LRO_OC_FLAG_10_EXP_CASE_E6 0x6
//! Constant E7 - TCP sequence number out of order
#define CONST_LRO_OC_FLAG_10_EXP_CASE_E7 0x7

//! Field SEG_NR - Number of segments collected
#define LRO_OC_FLAG_10_SEG_NR_POS 10
//! Field SEG_NR - Number of segments collected
#define LRO_OC_FLAG_10_SEG_NR_MASK 0x7C00u

//! Field TPL - Length of all segments collected
#define LRO_OC_FLAG_10_TPL_POS 15
//! Field TPL - Length of all segments collected
#define LRO_OC_FLAG_10_TPL_MASK 0x7FFF8000u

//! @}

//! \defgroup LRO_OC_OWNER_10 Register LRO_OC_OWNER_10 - Owner bit for the output context
//! @{

//! Register Offset (relative)
#define LRO_OC_OWNER_10 0x2AB0

//! Register Reset Value
#define LRO_OC_OWNER_10_RST 0x00000000u

//! Field OWNER - Owner ship of the context
#define LRO_OC_OWNER_10_OWNER_POS 31
//! Field OWNER - Owner ship of the context
#define LRO_OC_OWNER_10_OWNER_MASK 0x80000000u
//! Constant LRO - SW writes 1 to clear it to 0 .
#define CONST_LRO_OC_OWNER_10_OWNER_LRO 0x0
//! Constant SW - LRO HW writes '1' to give ownership to SW
#define CONST_LRO_OC_OWNER_10_OWNER_SW 0x1

//! @}

//! \defgroup LRO_OC_FLAG_11 Register LRO_OC_FLAG_11 - LRO Output Context Flag
//! @{

//! Register Offset (relative)
#define LRO_OC_FLAG_11 0x2BA8

//! Register Reset Value
#define LRO_OC_FLAG_11_RST 0x00000000u

//! Field AGG - Aggregation flag
#define LRO_OC_FLAG_11_AGG_POS 0
//! Field AGG - Aggregation flag
#define LRO_OC_FLAG_11_AGG_MASK 0x1u
//! Constant AGG0 - single packet session
#define CONST_LRO_OC_FLAG_11_AGG_AGG0 0x0
//! Constant AGG1 - Aggregation is performance; more than one segment
#define CONST_LRO_OC_FLAG_11_AGG_AGG1 0x1

//! Field TCP_TS - TCP Timestamp Option check performed indication
#define LRO_OC_FLAG_11_TCP_TS_POS 1
//! Field TCP_TS - TCP Timestamp Option check performed indication
#define LRO_OC_FLAG_11_TCP_TS_MASK 0x2u
//! Constant NIL - not done
#define CONST_LRO_OC_FLAG_11_TCP_TS_NIL 0x0
//! Constant TS_D - Done TCP Timestamp check
#define CONST_LRO_OC_FLAG_11_TCP_TS_TS_D 0x1

//! Field EXCPT - LRO indication of any exception is processed
#define LRO_OC_FLAG_11_EXCPT_POS 2
//! Field EXCPT - LRO indication of any exception is processed
#define LRO_OC_FLAG_11_EXCPT_MASK 0x4u
//! Constant NIL - No exception
#define CONST_LRO_OC_FLAG_11_EXCPT_NIL 0x0
//! Constant EXP - any exception happened
#define CONST_LRO_OC_FLAG_11_EXCPT_EXP 0x1

//! Field EXP_CASE - Exception Reason list
#define LRO_OC_FLAG_11_EXP_CASE_POS 3
//! Field EXP_CASE - Exception Reason list
#define LRO_OC_FLAG_11_EXP_CASE_MASK 0x78u
//! Constant E0 - Flush based on GSWIP indication given
#define CONST_LRO_OC_FLAG_11_EXP_CASE_E0 0x0
//! Constant E1 - TCP check sum error
#define CONST_LRO_OC_FLAG_11_EXP_CASE_E1 0x1
//! Constant E2 - FID mismatch(not used)
#define CONST_LRO_OC_FLAG_11_EXP_CASE_E2 0x2
//! Constant E3 - TCP zero length packet
#define CONST_LRO_OC_FLAG_11_EXP_CASE_E3 0x3
//! Constant E4 - TCP Timestamp check error
#define CONST_LRO_OC_FLAG_11_EXP_CASE_E4 0x4
//! Constant E5 - Excessive length
#define CONST_LRO_OC_FLAG_11_EXP_CASE_E5 0x5
//! Constant E6 - session timeout
#define CONST_LRO_OC_FLAG_11_EXP_CASE_E6 0x6
//! Constant E7 - TCP sequence number out of order
#define CONST_LRO_OC_FLAG_11_EXP_CASE_E7 0x7

//! Field SEG_NR - Number of segments collected
#define LRO_OC_FLAG_11_SEG_NR_POS 10
//! Field SEG_NR - Number of segments collected
#define LRO_OC_FLAG_11_SEG_NR_MASK 0x7C00u

//! Field TPL - Length of all segments collected
#define LRO_OC_FLAG_11_TPL_POS 15
//! Field TPL - Length of all segments collected
#define LRO_OC_FLAG_11_TPL_MASK 0x7FFF8000u

//! @}

//! \defgroup LRO_OC_OWNER_11 Register LRO_OC_OWNER_11 - Owner bit for the output context
//! @{

//! Register Offset (relative)
#define LRO_OC_OWNER_11 0x2BB0

//! Register Reset Value
#define LRO_OC_OWNER_11_RST 0x00000000u

//! Field OWNER - Owner ship of the context
#define LRO_OC_OWNER_11_OWNER_POS 31
//! Field OWNER - Owner ship of the context
#define LRO_OC_OWNER_11_OWNER_MASK 0x80000000u
//! Constant LRO - SW writes 1 to clear it to 0 .
#define CONST_LRO_OC_OWNER_11_OWNER_LRO 0x0
//! Constant SW - LRO HW writes '1' to give ownership to SW
#define CONST_LRO_OC_OWNER_11_OWNER_SW 0x1

//! @}

//! \defgroup LRO_OC_FLAG_12 Register LRO_OC_FLAG_12 - LRO Output Context Flag
//! @{

//! Register Offset (relative)
#define LRO_OC_FLAG_12 0x2CA8

//! Register Reset Value
#define LRO_OC_FLAG_12_RST 0x00000000u

//! Field AGG - Aggregation flag
#define LRO_OC_FLAG_12_AGG_POS 0
//! Field AGG - Aggregation flag
#define LRO_OC_FLAG_12_AGG_MASK 0x1u
//! Constant AGG0 - single packet session
#define CONST_LRO_OC_FLAG_12_AGG_AGG0 0x0
//! Constant AGG1 - Aggregation is performance; more than one segment
#define CONST_LRO_OC_FLAG_12_AGG_AGG1 0x1

//! Field TCP_TS - TCP Timestamp Option check performed indication
#define LRO_OC_FLAG_12_TCP_TS_POS 1
//! Field TCP_TS - TCP Timestamp Option check performed indication
#define LRO_OC_FLAG_12_TCP_TS_MASK 0x2u
//! Constant NIL - not done
#define CONST_LRO_OC_FLAG_12_TCP_TS_NIL 0x0
//! Constant TS_D - Done TCP Timestamp check
#define CONST_LRO_OC_FLAG_12_TCP_TS_TS_D 0x1

//! Field EXCPT - LRO indication of any exception is processed
#define LRO_OC_FLAG_12_EXCPT_POS 2
//! Field EXCPT - LRO indication of any exception is processed
#define LRO_OC_FLAG_12_EXCPT_MASK 0x4u
//! Constant NIL - No exception
#define CONST_LRO_OC_FLAG_12_EXCPT_NIL 0x0
//! Constant EXP - any exception happened
#define CONST_LRO_OC_FLAG_12_EXCPT_EXP 0x1

//! Field EXP_CASE - Exception Reason list
#define LRO_OC_FLAG_12_EXP_CASE_POS 3
//! Field EXP_CASE - Exception Reason list
#define LRO_OC_FLAG_12_EXP_CASE_MASK 0x78u
//! Constant E0 - Flush based on GSWIP indication given
#define CONST_LRO_OC_FLAG_12_EXP_CASE_E0 0x0
//! Constant E1 - TCP check sum error
#define CONST_LRO_OC_FLAG_12_EXP_CASE_E1 0x1
//! Constant E2 - FID mismatch(not used)
#define CONST_LRO_OC_FLAG_12_EXP_CASE_E2 0x2
//! Constant E3 - TCP zero length packet
#define CONST_LRO_OC_FLAG_12_EXP_CASE_E3 0x3
//! Constant E4 - TCP Timestamp check error
#define CONST_LRO_OC_FLAG_12_EXP_CASE_E4 0x4
//! Constant E5 - Excessive length
#define CONST_LRO_OC_FLAG_12_EXP_CASE_E5 0x5
//! Constant E6 - session timeout
#define CONST_LRO_OC_FLAG_12_EXP_CASE_E6 0x6
//! Constant E7 - TCP sequence number out of order
#define CONST_LRO_OC_FLAG_12_EXP_CASE_E7 0x7

//! Field SEG_NR - Number of segments collected
#define LRO_OC_FLAG_12_SEG_NR_POS 10
//! Field SEG_NR - Number of segments collected
#define LRO_OC_FLAG_12_SEG_NR_MASK 0x7C00u

//! Field TPL - Length of all segments collected
#define LRO_OC_FLAG_12_TPL_POS 15
//! Field TPL - Length of all segments collected
#define LRO_OC_FLAG_12_TPL_MASK 0x7FFF8000u

//! @}

//! \defgroup LRO_OC_OWNER_12 Register LRO_OC_OWNER_12 - Owner bit for the output context
//! @{

//! Register Offset (relative)
#define LRO_OC_OWNER_12 0x2CB0

//! Register Reset Value
#define LRO_OC_OWNER_12_RST 0x00000000u

//! Field OWNER - Owner ship of the context
#define LRO_OC_OWNER_12_OWNER_POS 31
//! Field OWNER - Owner ship of the context
#define LRO_OC_OWNER_12_OWNER_MASK 0x80000000u
//! Constant LRO - SW writes 1 to clear it to 0 .
#define CONST_LRO_OC_OWNER_12_OWNER_LRO 0x0
//! Constant SW - LRO HW writes '1' to give ownership to SW
#define CONST_LRO_OC_OWNER_12_OWNER_SW 0x1

//! @}

//! \defgroup LRO_OC_FLAG_13 Register LRO_OC_FLAG_13 - LRO Output Context Flag
//! @{

//! Register Offset (relative)
#define LRO_OC_FLAG_13 0x2DA8

//! Register Reset Value
#define LRO_OC_FLAG_13_RST 0x00000000u

//! Field AGG - Aggregation flag
#define LRO_OC_FLAG_13_AGG_POS 0
//! Field AGG - Aggregation flag
#define LRO_OC_FLAG_13_AGG_MASK 0x1u
//! Constant AGG0 - single packet session
#define CONST_LRO_OC_FLAG_13_AGG_AGG0 0x0
//! Constant AGG1 - Aggregation is performance; more than one segment
#define CONST_LRO_OC_FLAG_13_AGG_AGG1 0x1

//! Field TCP_TS - TCP Timestamp Option check performed indication
#define LRO_OC_FLAG_13_TCP_TS_POS 1
//! Field TCP_TS - TCP Timestamp Option check performed indication
#define LRO_OC_FLAG_13_TCP_TS_MASK 0x2u
//! Constant NIL - not done
#define CONST_LRO_OC_FLAG_13_TCP_TS_NIL 0x0
//! Constant TS_D - Done TCP Timestamp check
#define CONST_LRO_OC_FLAG_13_TCP_TS_TS_D 0x1

//! Field EXCPT - LRO indication of any exception is processed
#define LRO_OC_FLAG_13_EXCPT_POS 2
//! Field EXCPT - LRO indication of any exception is processed
#define LRO_OC_FLAG_13_EXCPT_MASK 0x4u
//! Constant NIL - No exception
#define CONST_LRO_OC_FLAG_13_EXCPT_NIL 0x0
//! Constant EXP - any exception happened
#define CONST_LRO_OC_FLAG_13_EXCPT_EXP 0x1

//! Field EXP_CASE - Exception Reason list
#define LRO_OC_FLAG_13_EXP_CASE_POS 3
//! Field EXP_CASE - Exception Reason list
#define LRO_OC_FLAG_13_EXP_CASE_MASK 0x78u
//! Constant E0 - Flush based on GSWIP indication given
#define CONST_LRO_OC_FLAG_13_EXP_CASE_E0 0x0
//! Constant E1 - TCP check sum error
#define CONST_LRO_OC_FLAG_13_EXP_CASE_E1 0x1
//! Constant E2 - FID mismatch(not used)
#define CONST_LRO_OC_FLAG_13_EXP_CASE_E2 0x2
//! Constant E3 - TCP zero length packet
#define CONST_LRO_OC_FLAG_13_EXP_CASE_E3 0x3
//! Constant E4 - TCP Timestamp check error
#define CONST_LRO_OC_FLAG_13_EXP_CASE_E4 0x4
//! Constant E5 - Excessive length
#define CONST_LRO_OC_FLAG_13_EXP_CASE_E5 0x5
//! Constant E6 - session timeout
#define CONST_LRO_OC_FLAG_13_EXP_CASE_E6 0x6
//! Constant E7 - TCP sequence number out of order
#define CONST_LRO_OC_FLAG_13_EXP_CASE_E7 0x7

//! Field SEG_NR - Number of segments collected
#define LRO_OC_FLAG_13_SEG_NR_POS 10
//! Field SEG_NR - Number of segments collected
#define LRO_OC_FLAG_13_SEG_NR_MASK 0x7C00u

//! Field TPL - Length of all segments collected
#define LRO_OC_FLAG_13_TPL_POS 15
//! Field TPL - Length of all segments collected
#define LRO_OC_FLAG_13_TPL_MASK 0x7FFF8000u

//! @}

//! \defgroup LRO_OC_OWNER_13 Register LRO_OC_OWNER_13 - Owner bit for the output context
//! @{

//! Register Offset (relative)
#define LRO_OC_OWNER_13 0x2DB0

//! Register Reset Value
#define LRO_OC_OWNER_13_RST 0x00000000u

//! Field OWNER - Owner ship of the context
#define LRO_OC_OWNER_13_OWNER_POS 31
//! Field OWNER - Owner ship of the context
#define LRO_OC_OWNER_13_OWNER_MASK 0x80000000u
//! Constant LRO - SW writes 1 to clear it to 0 .
#define CONST_LRO_OC_OWNER_13_OWNER_LRO 0x0
//! Constant SW - LRO HW writes '1' to give ownership to SW
#define CONST_LRO_OC_OWNER_13_OWNER_SW 0x1

//! @}

//! \defgroup LRO_OC_FLAG_14 Register LRO_OC_FLAG_14 - LRO Output Context Flag
//! @{

//! Register Offset (relative)
#define LRO_OC_FLAG_14 0x2EA8

//! Register Reset Value
#define LRO_OC_FLAG_14_RST 0x00000000u

//! Field AGG - Aggregation flag
#define LRO_OC_FLAG_14_AGG_POS 0
//! Field AGG - Aggregation flag
#define LRO_OC_FLAG_14_AGG_MASK 0x1u
//! Constant AGG0 - single packet session
#define CONST_LRO_OC_FLAG_14_AGG_AGG0 0x0
//! Constant AGG1 - Aggregation is performance; more than one segment
#define CONST_LRO_OC_FLAG_14_AGG_AGG1 0x1

//! Field TCP_TS - TCP Timestamp Option check performed indication
#define LRO_OC_FLAG_14_TCP_TS_POS 1
//! Field TCP_TS - TCP Timestamp Option check performed indication
#define LRO_OC_FLAG_14_TCP_TS_MASK 0x2u
//! Constant NIL - not done
#define CONST_LRO_OC_FLAG_14_TCP_TS_NIL 0x0
//! Constant TS_D - Done TCP Timestamp check
#define CONST_LRO_OC_FLAG_14_TCP_TS_TS_D 0x1

//! Field EXCPT - LRO indication of any exception is processed
#define LRO_OC_FLAG_14_EXCPT_POS 2
//! Field EXCPT - LRO indication of any exception is processed
#define LRO_OC_FLAG_14_EXCPT_MASK 0x4u
//! Constant NIL - No exception
#define CONST_LRO_OC_FLAG_14_EXCPT_NIL 0x0
//! Constant EXP - any exception happened
#define CONST_LRO_OC_FLAG_14_EXCPT_EXP 0x1

//! Field EXP_CASE - Exception Reason list
#define LRO_OC_FLAG_14_EXP_CASE_POS 3
//! Field EXP_CASE - Exception Reason list
#define LRO_OC_FLAG_14_EXP_CASE_MASK 0x78u
//! Constant E0 - Flush based on GSWIP indication given
#define CONST_LRO_OC_FLAG_14_EXP_CASE_E0 0x0
//! Constant E1 - TCP check sum error
#define CONST_LRO_OC_FLAG_14_EXP_CASE_E1 0x1
//! Constant E2 - FID mismatch(not used)
#define CONST_LRO_OC_FLAG_14_EXP_CASE_E2 0x2
//! Constant E3 - TCP zero length packet
#define CONST_LRO_OC_FLAG_14_EXP_CASE_E3 0x3
//! Constant E4 - TCP Timestamp check error
#define CONST_LRO_OC_FLAG_14_EXP_CASE_E4 0x4
//! Constant E5 - Excessive length
#define CONST_LRO_OC_FLAG_14_EXP_CASE_E5 0x5
//! Constant E6 - session timeout
#define CONST_LRO_OC_FLAG_14_EXP_CASE_E6 0x6
//! Constant E7 - TCP sequence number out of order
#define CONST_LRO_OC_FLAG_14_EXP_CASE_E7 0x7

//! Field SEG_NR - Number of segments collected
#define LRO_OC_FLAG_14_SEG_NR_POS 10
//! Field SEG_NR - Number of segments collected
#define LRO_OC_FLAG_14_SEG_NR_MASK 0x7C00u

//! Field TPL - Length of all segments collected
#define LRO_OC_FLAG_14_TPL_POS 15
//! Field TPL - Length of all segments collected
#define LRO_OC_FLAG_14_TPL_MASK 0x7FFF8000u

//! @}

//! \defgroup LRO_OC_OWNER_14 Register LRO_OC_OWNER_14 - Owner bit for the output context
//! @{

//! Register Offset (relative)
#define LRO_OC_OWNER_14 0x2EB0

//! Register Reset Value
#define LRO_OC_OWNER_14_RST 0x00000000u

//! Field OWNER - Owner ship of the context
#define LRO_OC_OWNER_14_OWNER_POS 31
//! Field OWNER - Owner ship of the context
#define LRO_OC_OWNER_14_OWNER_MASK 0x80000000u
//! Constant LRO - SW writes 1 to clear it to 0 .
#define CONST_LRO_OC_OWNER_14_OWNER_LRO 0x0
//! Constant SW - LRO HW writes '1' to give ownership to SW
#define CONST_LRO_OC_OWNER_14_OWNER_SW 0x1

//! @}

//! \defgroup LRO_OC_FLAG_15 Register LRO_OC_FLAG_15 - LRO Output Context Flag
//! @{

//! Register Offset (relative)
#define LRO_OC_FLAG_15 0x2FA8

//! Register Reset Value
#define LRO_OC_FLAG_15_RST 0x00000000u

//! Field AGG - Aggregation flag
#define LRO_OC_FLAG_15_AGG_POS 0
//! Field AGG - Aggregation flag
#define LRO_OC_FLAG_15_AGG_MASK 0x1u
//! Constant AGG0 - single packet session
#define CONST_LRO_OC_FLAG_15_AGG_AGG0 0x0
//! Constant AGG1 - Aggregation is performance; more than one segment
#define CONST_LRO_OC_FLAG_15_AGG_AGG1 0x1

//! Field TCP_TS - TCP Timestamp Option check performed indication
#define LRO_OC_FLAG_15_TCP_TS_POS 1
//! Field TCP_TS - TCP Timestamp Option check performed indication
#define LRO_OC_FLAG_15_TCP_TS_MASK 0x2u
//! Constant NIL - not done
#define CONST_LRO_OC_FLAG_15_TCP_TS_NIL 0x0
//! Constant TS_D - Done TCP Timestamp check
#define CONST_LRO_OC_FLAG_15_TCP_TS_TS_D 0x1

//! Field EXCPT - LRO indication of any exception is processed
#define LRO_OC_FLAG_15_EXCPT_POS 2
//! Field EXCPT - LRO indication of any exception is processed
#define LRO_OC_FLAG_15_EXCPT_MASK 0x4u
//! Constant NIL - No exception
#define CONST_LRO_OC_FLAG_15_EXCPT_NIL 0x0
//! Constant EXP - any exception happened
#define CONST_LRO_OC_FLAG_15_EXCPT_EXP 0x1

//! Field EXP_CASE - Exception Reason list
#define LRO_OC_FLAG_15_EXP_CASE_POS 3
//! Field EXP_CASE - Exception Reason list
#define LRO_OC_FLAG_15_EXP_CASE_MASK 0x78u
//! Constant E0 - Flush based on GSWIP indication given
#define CONST_LRO_OC_FLAG_15_EXP_CASE_E0 0x0
//! Constant E1 - TCP check sum error
#define CONST_LRO_OC_FLAG_15_EXP_CASE_E1 0x1
//! Constant E2 - FID mismatch(not used)
#define CONST_LRO_OC_FLAG_15_EXP_CASE_E2 0x2
//! Constant E3 - TCP zero length packet
#define CONST_LRO_OC_FLAG_15_EXP_CASE_E3 0x3
//! Constant E4 - TCP Timestamp check error
#define CONST_LRO_OC_FLAG_15_EXP_CASE_E4 0x4
//! Constant E5 - Excessive length
#define CONST_LRO_OC_FLAG_15_EXP_CASE_E5 0x5
//! Constant E6 - session timeout
#define CONST_LRO_OC_FLAG_15_EXP_CASE_E6 0x6
//! Constant E7 - TCP sequence number out of order
#define CONST_LRO_OC_FLAG_15_EXP_CASE_E7 0x7

//! Field SEG_NR - Number of segments collected
#define LRO_OC_FLAG_15_SEG_NR_POS 10
//! Field SEG_NR - Number of segments collected
#define LRO_OC_FLAG_15_SEG_NR_MASK 0x7C00u

//! Field TPL - Length of all segments collected
#define LRO_OC_FLAG_15_TPL_POS 15
//! Field TPL - Length of all segments collected
#define LRO_OC_FLAG_15_TPL_MASK 0x7FFF8000u

//! @}

//! \defgroup LRO_OC_OWNER_15 Register LRO_OC_OWNER_15 - Owner bit for the output context
//! @{

//! Register Offset (relative)
#define LRO_OC_OWNER_15 0x2FB0

//! Register Reset Value
#define LRO_OC_OWNER_15_RST 0x00000000u

//! Field OWNER - Owner ship of the context
#define LRO_OC_OWNER_15_OWNER_POS 31
//! Field OWNER - Owner ship of the context
#define LRO_OC_OWNER_15_OWNER_MASK 0x80000000u
//! Constant LRO - SW writes 1 to clear it to 0 .
#define CONST_LRO_OC_OWNER_15_OWNER_LRO 0x0
//! Constant SW - LRO HW writes '1' to give ownership to SW
#define CONST_LRO_OC_OWNER_15_OWNER_SW 0x1

//! @}

//! \defgroup LRO_EXP_EFLAG Register LRO_EXP_EFLAG - LRO Exception Entry Flag
//! @{

//! Register Offset (relative)
#define LRO_EXP_EFLAG 0x4100

//! Register Reset Value
#define LRO_EXP_EFLAG_RST 0x00000000u

//! Field EXP - one entry per LRO command port for exception entry
#define LRO_EXP_EFLAG_EXP_POS 0
//! Field EXP - one entry per LRO command port for exception entry
#define LRO_EXP_EFLAG_EXP_MASK 0xFFFFu

//! Field UNMATCH - unmatched entry flag
#define LRO_EXP_EFLAG_UNMATCH_POS 16
//! Field UNMATCH - unmatched entry flag
#define LRO_EXP_EFLAG_UNMATCH_MASK 0xFF0000u

//! @}

//! @}

#endif
