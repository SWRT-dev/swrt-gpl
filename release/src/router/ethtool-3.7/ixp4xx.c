/*
 * Copyright (c) 2006 Christian Hohnstaed <chohnstaedt@innominate.com>
 * This file is released under the GPLv2
 */

#include <stdio.h>
#include "internal.h"

#ifndef BIT
#define BIT(x) (1<<x)
#endif

#define TX_CNTRL1_TX_EN         BIT(0)
#define TX_CNTRL1_DUPLEX        BIT(1)
#define TX_CNTRL1_RETRY         BIT(2)
#define TX_CNTRL1_PAD_EN        BIT(3)
#define TX_CNTRL1_FCS_EN        BIT(4)
#define TX_CNTRL1_2DEFER        BIT(5)
#define TX_CNTRL1_RMII          BIT(6)

/* TX Control Register 2 */
#define TX_CNTRL2_RETRIES_MASK  0xf

/* RX Control Register 1 */
#define RX_CNTRL1_RX_EN         BIT(0)
#define RX_CNTRL1_PADSTRIP_EN   BIT(1)
#define RX_CNTRL1_CRC_EN        BIT(2)
#define RX_CNTRL1_PAUSE_EN      BIT(3)
#define RX_CNTRL1_LOOP_EN       BIT(4)
#define RX_CNTRL1_ADDR_FLTR_EN  BIT(5)
#define RX_CNTRL1_RX_RUNT_EN    BIT(6)
#define RX_CNTRL1_BCAST_DIS     BIT(7)

/* Core Control Register */
#define CORE_RESET              BIT(0)
#define CORE_RX_FIFO_FLUSH      BIT(1)
#define CORE_TX_FIFO_FLUSH      BIT(2)
#define CORE_SEND_JAM           BIT(3)
#define CORE_MDC_EN             BIT(4)

#define MAC "%02x:%02x:%02x:%02x:%02x:%02x"
#define MAC_DATA(d) (d)[0], (d)[1], (d)[2], (d)[3], (d)[4], (d)[5]

int ixp4xx_dump_regs(struct ethtool_drvinfo *info, struct ethtool_regs *regs)
{
	u8 *data = regs->data;

	fprintf(stdout,
		"TXctrl:          0x%02x:0x%02x\n"
		"    Enable:      %s\n"
		"    Duplex:      %s\n"
		"    Retry:       %s (%d)\n"
		"    Padding:     %s\n"
		"    Frame check: %s\n"
		"    TX deferral: %s\n"
		"    Connection:  %s\n"
		"\n",
		data[0], data[1],
		data[0] & TX_CNTRL1_TX_EN   ? "yes"      : "no",
		data[0] & TX_CNTRL1_DUPLEX  ? "half"     : "full",
		data[0] & TX_CNTRL1_RETRY   ? "enabled"  : "disabled",
		data[1] & TX_CNTRL2_RETRIES_MASK,
		data[0] & TX_CNTRL1_PAD_EN  ? "enabled"  : "disabled",
		data[0] & TX_CNTRL1_FCS_EN  ? "enabled"  : "disabled",
		data[0] & TX_CNTRL1_2DEFER  ? "two-part" : "one-part",
		data[0] & TX_CNTRL1_RMII    ? "RMII"     : "Full MII"
	);

	fprintf(stdout,
		"RXctrl:          0x%02x\n"
		"    Enable:      %s\n"
		"    Pad strip:   %s\n"
		"    CRC check:   %s\n"
		"    Pause:       %s\n"
		"    Loop:        %s\n"
		"    Promiscous:  %s\n"
		"    Runt frames: %s\n"
		"    Broadcast:   %s\n"
		"\n",
		data[2],
		data[2] & RX_CNTRL1_RX_EN        ? "yes"      : "no",
		data[2] & RX_CNTRL1_PADSTRIP_EN  ? "enabled"  : "disabled",
		data[2] & RX_CNTRL1_CRC_EN       ? "enabled"  : "disabled",
		data[2] & RX_CNTRL1_PAUSE_EN     ? "enabled"  : "disabled",
		data[2] & RX_CNTRL1_LOOP_EN      ? "enabled"  : "disabled",
		data[2] & RX_CNTRL1_ADDR_FLTR_EN ? "disabled" : "enabled",
		data[2] & RX_CNTRL1_RX_RUNT_EN   ? "forward"  : "discard",
		data[2] & RX_CNTRL1_BCAST_DIS    ? "disabled" : "enabled"
	);
	fprintf(stdout,
		"Core control:    0x%02x\n"
		"    Core state:  %s\n"
		"    RX fifo:     %s\n"
		"    TX fifo:     %s\n"
		"    Send jam:    %s\n"
		"    MDC clock    %s\n"
		"\n",
		data[32],
		data[32] & CORE_RESET          ? "reset"  : "normal operation",
		data[32] & CORE_RX_FIFO_FLUSH  ? "flush"  : "ok",
		data[32] & CORE_TX_FIFO_FLUSH  ? "flush"  : "ok",
		data[32] & CORE_SEND_JAM       ? "yes"    : "no",
		data[32] & CORE_MDC_EN         ? "output" : "input"
	);
	fprintf(stdout,
		"MAC addresses: \n"
		"  Multicast mask:     " MAC "\n"
		"  Multicast address:  " MAC "\n"
		"  Unicast address:    " MAC "\n"
		"\n",
		MAC_DATA(data+13), MAC_DATA(data+19), MAC_DATA(data+26)
	);
	fprintf(stdout,
		"Random seed:      0x%02x\n"
		"Threshold empty:   %3d\n"
		"Threshold full:    %3d\n"
		"TX buffer size:    %3d\n"
		"TX deferral:       %3d\n"
		"RX deferral:       %3d\n"
		"TX two deferral 1: %3d\n"
		"TX two deferral 2: %3d\n"
		"Slot time:         %3d\n"
		"Internal clock:    %3d\n"
		"\n",
		data[4], data[5], data[6], data[7], data[8], data[9],
		data[10], data[11], data[12], data[25]
	);

	return 0;
}
