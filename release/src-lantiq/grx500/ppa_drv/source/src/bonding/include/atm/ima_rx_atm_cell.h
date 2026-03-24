
/****************************************************************************** 
         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
 
  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.
 
******************************************************************************/

#ifndef __IMA_RX_ATM_CELL_H__
#define __IMA_RX_ATM_CELL_H__

#define GETVPI(cell) (((cell)->vpi_hi << 4) | ((cell)->vpi_lo))
#define GETVCI(cell) (((cell)->vci_hi << 4) | ((cell)->vci_lo))
#define GETSID(cell, is8bit) ((is8bit) ? GET8BITSID((cell)) : GET12BITSID((cell)))
#define GET8BITSID(cell) (((cell)->sid8_hi << 4) | ((cell)->sid8_lo))
#define GET12BITSID(cell) (((cell)->sid12_hi << 8) | ((cell)->sid8_hi << 4) | ((cell)->sid8_lo))

#define SETVPI(cell, vpi) \
	do { \
		(cell)->vpi_hi = ((vpi) & 0xff) >> 4; \
		(cell)->vpi_lo = (vpi) & 0x0f; \
	} while (0)

#define SETVCI(cell, vci) \
	do { \
		(cell)->vci_hi = ((vci) & 0xff) >> 4; \
		(cell)->vci_lo = (vci) & 0x0f; \
	} while (0)

#define SETSID(cell, sid, is8bit) \
	do { \
		if ((is8bit)) \
			SET8BITSID((cell), (sid)); \
		else \
			SET12BITSID((cell), (sid)); \
	} while (0)

#define SET8BITSID(cell, sid) \
	do { \
		(cell)->sid8_hi = ((sid) & 0xff) >> 4; \
		(cell)->sid8_lo = (sid) & 0x0f; \
	} while (0)

#define SET12BITSID(cell, sid) \
	do { \
		(cell)->sid12_hi = ((sid) & 0xf00) >> 8; \
		(cell)->sid8_hi = ((sid) & 0xff) >> 4; \
		(cell)->sid8_lo = (sid) & 0x0f; \
	} while (0)

#define PRINTCELL(cell) \
	do { \
		printf("VPI: 0x%02X (%d)\n", GETVPI((cell)), GETVPI((cell))); \
		printf("VCI: 0x%02X (%d)\n", GETVCI((cell)), GETVCI((cell))); \
		printf("SID: 0x%03X (%d)\n", GETSID((cell)), GETSID((cell))); \
		printf("PT: %d %d %d\n", (cell)->pt3, (cell)->pt2, (cell)->pt1); \
		printf("CLP: %d\n", (cell)->clp); \
		printf("HEC: 0x%02X (%d)\n", (cell)->hec, (cell)->hec); \
	} while (0)


typedef struct {

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__

	// BYTE: 1 OF ATM HEADER
	unsigned char vpi_hi:4;
	unsigned char sid12_hi:4;

	// BYTE: 2 OF ATM HEADER
	unsigned char sid8_hi:4;
	unsigned char vpi_lo:4;

	// BYTE: 3 OF ATM HEADER
	unsigned char vci_hi:4;
	unsigned char sid8_lo:4;

	// BYTE: 4 OF ATM HEADER
	unsigned char clp:1;
	unsigned char pt1:1;
	unsigned char pt2:1;
	unsigned char pt3:1;
	unsigned char vci_lo:4;

#else

	// BYTE: 1 OF ATM HEADER
	unsigned char sid12_hi:4;
	unsigned char vpi_hi:4;

	// BYTE: 2 OF ATM HEADER
	unsigned char vpi_lo:4;
	unsigned char sid8_hi:4;

	// BYTE: 3 OF ATM HEADER
	unsigned char sid8_lo:4;
	unsigned char vci_hi:4;

	// BYTE: 4 OF ATM HEADER
	unsigned char vci_lo:4;
	unsigned char pt3:1;
	unsigned char pt2:1;
	unsigned char pt1:1;
	unsigned char clp:1;

#endif

	// BYTE: 5 OF ATM HEADER
	unsigned char hec;

	// VRX518 3 byte padding
	unsigned char padding[3];

	// From BYTE 6 of ATM PAYLOAD
	unsigned char payload[0];

} __attribute__((packed)) ima_rx_atm_header_t;

#endif /* __IMA_RX_ATM_CELL_H__ */
