/* Copyright 2001 Sun Microsystems (thockin@sun.com) */
#include <stdio.h>
#include <stdlib.h>
#include "internal.h"

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#define HW_REVID(b31, b30, b29, b28, b27, b26, b23, b22) \
	(b31<<31 | b30<<30 | b29<<29 | b28<<28 | \
	 b27<<27 | b26<<26 | b23<<23 | b22<<22)

enum chip_type {
	RTLNONE,
	RTL8139,
	RTL8139_K,
	RTL8139A,
	RTL8139A_G,
	RTL8139B,
	RTL8130,
	RTL8139C,
	RTL8100,
	RTL8100B_8139D,
	RTL8139Cp,
	RTL8101,
	RTL8169,
	RTL8169S,
	RTL8110S,
	RTL8169_8110SB,
	RTL8169_8110SCd,
	RTL8169_8110SCe,
	RTL8168_8111Bb,
	RTL8168_8111Bef,
	RTL8101Ebc,
	RTL8100E1,
	RTL8100E2,
};

enum {
	chip_type_mask = HW_REVID(1, 1, 1, 1, 1, 1, 1, 1)
};

static struct chip_info {
	const char *name;
	u32 id_mask;
} rtl_info_tbl[] = {
	{ "RTL-8139",		HW_REVID(0, 1, 0, 0, 0, 0, 0, 0) },
	{ "RTL-8139-K",		HW_REVID(0, 1, 1, 0, 0, 0, 0, 0) },
	{ "RTL-8139A",		HW_REVID(0, 1, 1, 1, 0, 0, 0, 0) },
	{ "RTL-8139A-G",	HW_REVID(0, 1, 1, 1, 0, 0, 1, 0) },
	{ "RTL-8139B",		HW_REVID(0, 1, 1, 1, 1, 0, 0, 0) },
	{ "RTL-8130",		HW_REVID(0, 1, 1, 1, 1, 1, 0, 0) },
	{ "RTL-8139C",		HW_REVID(0, 1, 1, 1, 0, 1, 0, 0) },
	{ "RTL-8100",		HW_REVID(0, 1, 1, 1, 1, 0, 1, 0) },
	{ "RTL-8100B/8139D",	HW_REVID(0, 1, 1, 1, 0, 1, 0, 1) },
	{ "RTL-8139C+",		HW_REVID(0, 1, 1, 1, 0, 1, 1, 0) },
	{ "RTL-8101",		HW_REVID(0, 1, 1, 1, 0, 1, 1, 1) },
	{ "RTL-8169",		HW_REVID(0, 0, 0, 0, 0, 0, 0, 0) },
	{ "RTL-8169S",		HW_REVID(0, 0, 0, 0, 0, 0, 1, 0) },
	{ "RTL-8110S",		HW_REVID(0, 0, 0, 0, 0, 1, 0, 0) },
	{ "RTL-8169/8110SB",	HW_REVID(0, 0, 0, 1, 0, 0, 0, 0) },
	{ "RTL-8169/8110SCd",	HW_REVID(0, 0, 0, 1, 1, 0, 0, 0) },
	{ "RTL-8169/8110SCe",	HW_REVID(1, 0, 0, 1, 1, 0, 0, 0) },
	{ "RTL-8168/8111Bb",	HW_REVID(0, 0, 1, 1, 0, 0, 0, 0) },
	{ "RTL-8168/8111Bef",	HW_REVID(0, 0, 1, 1, 1, 0, 0, 0) },
	{ "RTL-8101Ebc",	HW_REVID(0, 0, 1, 1, 0, 1, 0, 0) },
	{ "RTL-8100E(1)",	HW_REVID(0, 0, 1, 1, 0, 0, 1, 0) },
	{ "RTL-8100E(2)",	HW_REVID(0, 0, 1, 1, 1, 0, 1, 0) },
	{ }
};

static void
print_intr_bits(u16 mask)
{
	fprintf(stdout,
		"      %s%s%s%s%s%s%s%s%s%s%s\n",
		mask & (1 << 15)	? "SERR " : "",
		mask & (1 << 14)	? "TimeOut " : "",
		mask & (1 << 8)		? "SWInt " : "",
		mask & (1 << 7)		? "TxNoBuf " : "",
		mask & (1 << 6)		? "RxFIFO " : "",
		mask & (1 << 5)		? "LinkChg " : "",
		mask & (1 << 4)		? "RxNoBuf " : "",
		mask & (1 << 3)		? "TxErr " : "",
		mask & (1 << 2)		? "TxOK " : "",
		mask & (1 << 1)		? "RxErr " : "",
		mask & (1 << 0)		? "RxOK " : "");
}

int
realtek_dump_regs(struct ethtool_drvinfo *info, struct ethtool_regs *regs)
{
	u32 *data = (u32 *) regs->data;
	u8 *data8 = (u8 *) regs->data;
	u32 v;
	struct chip_info *ci;
	unsigned int board_type = RTLNONE, i;

	v = data[0x40 >> 2] & chip_type_mask;

	ci = &rtl_info_tbl[0];
	while (ci->name) {
		if (v == ci->id_mask)
			break;
		ci++;
	}
	if (v != ci->id_mask) {
		fprintf(stderr, "Unknown RealTek chip (mask: 0x%08x)\n", v);
		return 91;
	}
	for (i = 0; i < ARRAY_SIZE(rtl_info_tbl); i++) {
		if (ci == &rtl_info_tbl[i])
			board_type = i + 1;
	}
	if (board_type == RTLNONE)
		abort();

	fprintf(stdout,
		"RealTek %s registers:\n"
		"--------------------------------------------------------\n",
		ci->name);

	fprintf(stdout,
		"0x00: MAC Address                      %02x:%02x:%02x:%02x:%02x:%02x\n",
		data8[0x00],
		data8[0x01],
		data8[0x02],
		data8[0x03],
		data8[0x04],
		data8[0x05]);

	fprintf(stdout,
		"0x08: Multicast Address Filter     0x%08x 0x%08x\n",
		data[0x08 >> 2],
		data[0x0c >> 2]);

	if (board_type == RTL8139Cp || board_type >= RTL8169) {
	fprintf(stdout,
		"0x10: Dump Tally Counter Command   0x%08x 0x%08x\n",
		data[0x10 >> 2],
		data[0x14 >> 2]);

	fprintf(stdout,
		"0x20: Tx Normal Priority Ring Addr 0x%08x 0x%08x\n",
		data[0x20 >> 2],
		data[0x24 >> 2]);

	fprintf(stdout,
		"0x28: Tx High Priority Ring Addr   0x%08x 0x%08x\n",
		data[0x28 >> 2],
		data[0x2C >> 2]);
	} else {
	fprintf(stdout,
		"0x10: Transmit Status Desc 0                  0x%08x\n"
		"0x14: Transmit Status Desc 1                  0x%08x\n"
		"0x18: Transmit Status Desc 2                  0x%08x\n"
		"0x1C: Transmit Status Desc 3                  0x%08x\n",
		data[0x10 >> 2],
		data[0x14 >> 2],
		data[0x18 >> 2],
		data[0x1C >> 2]);
	fprintf(stdout,
		"0x20: Transmit Start Addr  0                  0x%08x\n"
		"0x24: Transmit Start Addr  1                  0x%08x\n"
		"0x28: Transmit Start Addr  2                  0x%08x\n"
		"0x2C: Transmit Start Addr  3                  0x%08x\n",
		data[0x20 >> 2],
		data[0x24 >> 2],
		data[0x28 >> 2],
		data[0x2C >> 2]);
	}

	if (board_type != RTL8168_8111Bb && board_type != RTL8168_8111Bef) {
	if (board_type >= RTL8169) {
	fprintf(stdout,
		"0x30: Flash memory read/write                 0x%08x\n",
		data[0x30 >> 2]);
	} else {
	fprintf(stdout,
		"0x30: Rx buffer addr (C mode)                 0x%08x\n",
		data[0x30 >> 2]);
	}
	}

	v = data8[0x36];
	fprintf(stdout,
		"0x34: Early Rx Byte Count                       %8u\n"
		"0x36: Early Rx Status                               0x%02x\n",
		data[0x34 >> 2] & 0xffff,
		v);

	if (v & 0xf) {
	fprintf(stdout,
		"      %s%s%s%s\n",
		v & (1 << 3) ? "ERxGood " : "",
		v & (1 << 2) ? "ERxBad " : "",
		v & (1 << 1) ? "ERxOverWrite " : "",
		v & (1 << 0) ? "ERxOK " : "");
	}

	v = data8[0x37];
	fprintf(stdout,
		"0x37: Command                                       0x%02x\n"
		"      Rx %s, Tx %s%s\n",
		data8[0x37],
		v & (1 << 3) ? "on" : "off",
		v & (1 << 2) ? "on" : "off",
		v & (1 << 4) ? ", RESET" : "");

	if (board_type < RTL8169) {
	fprintf(stdout,
		"0x38: Current Address of Packet Read (C mode)     0x%04x\n"
		"0x3A: Current Rx buffer address (C mode)          0x%04x\n",
		data[0x38 >> 2] & 0xffff,
		data[0x38 >> 2] >> 16);
	}

	fprintf(stdout,
		"0x3C: Interrupt Mask                              0x%04x\n",
		data[0x3c >> 2] & 0xffff);
	print_intr_bits(data[0x3c >> 2] & 0xffff);
	fprintf(stdout,
		"0x3E: Interrupt Status                            0x%04x\n",
		data[0x3c >> 2] >> 16);
	print_intr_bits(data[0x3c >> 2] >> 16);

	fprintf(stdout,
		"0x40: Tx Configuration                        0x%08x\n"
		"0x44: Rx Configuration                        0x%08x\n"
		"0x48: Timer count                             0x%08x\n"
		"0x4C: Missed packet counter                     0x%06x\n",
		data[0x40 >> 2],
		data[0x44 >> 2],
		data[0x48 >> 2],
		data[0x4C >> 2] & 0xffffff);

	fprintf(stdout,
		"0x50: EEPROM Command                                0x%02x\n"
		"0x51: Config 0                                      0x%02x\n"
		"0x52: Config 1                                      0x%02x\n",
		data8[0x50],
		data8[0x51],
		data8[0x52]);

	if (board_type >= RTL8169) {
	fprintf(stdout,
		"0x53: Config 2                                      0x%02x\n"
		"0x54: Config 3                                      0x%02x\n"
		"0x55: Config 4                                      0x%02x\n"
		"0x56: Config 5                                      0x%02x\n",
		data8[0x53],
		data8[0x54],
		data8[0x55],
		data8[0x56]);
	fprintf(stdout,
		"0x58: Timer interrupt                         0x%08x\n",
		data[0x58 >> 2]);
	}
	else {
	if (board_type >= RTL8139A) {
	fprintf(stdout,
		"0x54: Timer interrupt                         0x%08x\n",
		data[0x54 >> 2]);
	}
	fprintf(stdout,
		"0x58: Media status                                  0x%02x\n",
		data8[0x58]);
	if (board_type >= RTL8139A) {
	fprintf(stdout,
		"0x59: Config 3                                      0x%02x\n",
		data8[0x59]);
	}
	if (board_type >= RTL8139B) {
	fprintf(stdout,
		"0x5A: Config 4                                      0x%02x\n",
		data8[0x5A]);
	}
	}

	fprintf(stdout,
		"0x5C: Multiple Interrupt Select                   0x%04x\n",
		data[0x5c >> 2] & 0xffff);

	if (board_type >= RTL8169) {
	fprintf(stdout,
		"0x60: PHY access                              0x%08x\n",
		data[0x60 >> 2]);

	if (board_type != RTL8168_8111Bb && board_type != RTL8168_8111Bef) {
	fprintf(stdout,
		"0x64: TBI control and status                  0x%08x\n",
		data[0x64 >> 2]);
	fprintf(stdout,
		"0x68: TBI Autonegotiation advertisement (ANAR)    0x%04x\n"
		"0x6A: TBI Link partner ability (LPAR)             0x%04x\n",
		data[0x68 >> 2] & 0xffff,
		data[0x68 >> 2] >> 16);
	}

	fprintf(stdout,
		"0x6C: PHY status                                    0x%02x\n",
		data8[0x6C]);

	fprintf(stdout,
		"0x84: PM wakeup frame 0            0x%08x 0x%08x\n"
		"0x8C: PM wakeup frame 1            0x%08x 0x%08x\n",
		data[0x84 >> 2],
		data[0x88 >> 2],
		data[0x8C >> 2],
		data[0x90 >> 2]);

	fprintf(stdout,
		"0x94: PM wakeup frame 2 (low)      0x%08x 0x%08x\n"
		"0x9C: PM wakeup frame 2 (high)     0x%08x 0x%08x\n",
		data[0x94 >> 2],
		data[0x98 >> 2],
		data[0x9C >> 2],
		data[0xA0 >> 2]);

	fprintf(stdout,
		"0xA4: PM wakeup frame 3 (low)      0x%08x 0x%08x\n"
		"0xAC: PM wakeup frame 3 (high)     0x%08x 0x%08x\n",
		data[0xA4 >> 2],
		data[0xA8 >> 2],
		data[0xAC >> 2],
		data[0xB0 >> 2]);

	fprintf(stdout,
		"0xB4: PM wakeup frame 4 (low)      0x%08x 0x%08x\n"
		"0xBC: PM wakeup frame 4 (high)     0x%08x 0x%08x\n",
		data[0xB4 >> 2],
		data[0xB8 >> 2],
		data[0xBC >> 2],
		data[0xC0 >> 2]);

	fprintf(stdout,
		"0xC4: Wakeup frame 0 CRC                          0x%04x\n"
		"0xC6: Wakeup frame 1 CRC                          0x%04x\n"
		"0xC8: Wakeup frame 2 CRC                          0x%04x\n"
		"0xCA: Wakeup frame 3 CRC                          0x%04x\n"
		"0xCC: Wakeup frame 4 CRC                          0x%04x\n",
		data[0xC4 >> 2] & 0xffff,
		data[0xC4 >> 2] >> 16,
		data[0xC8 >> 2] & 0xffff,
		data[0xC8 >> 2] >> 16,
		data[0xCC >> 2] & 0xffff);
	fprintf(stdout,
		"0xDA: RX packet maximum size                      0x%04x\n",
		data[0xD8 >> 2] >> 16);
	}
	else {
	fprintf(stdout,
		"0x5E: PCI revision id                               0x%02x\n",
		data8[0x5e]);
	fprintf(stdout,
		"0x60: Transmit Status of All Desc (C mode)        0x%04x\n"
		"0x62: MII Basic Mode Control Register             0x%04x\n",
		data[0x60 >> 2] & 0xffff,
		data[0x60 >> 2] >> 16);
	fprintf(stdout,
		"0x64: MII Basic Mode Status Register              0x%04x\n"
		"0x66: MII Autonegotiation Advertising             0x%04x\n",
		data[0x64 >> 2] & 0xffff,
		data[0x64 >> 2] >> 16);
	fprintf(stdout,
		"0x68: MII Link Partner Ability                    0x%04x\n"
		"0x6A: MII Expansion                               0x%04x\n",
		data[0x68 >> 2] & 0xffff,
		data[0x68 >> 2] >> 16);
	fprintf(stdout,
		"0x6C: MII Disconnect counter                      0x%04x\n"
		"0x6E: MII False carrier sense counter             0x%04x\n",
		data[0x6C >> 2] & 0xffff,
		data[0x6C >> 2] >> 16);
	fprintf(stdout,
		"0x70: MII Nway test                               0x%04x\n"
		"0x72: MII RX_ER counter                           0x%04x\n",
		data[0x70 >> 2] & 0xffff,
		data[0x70 >> 2] >> 16);
	fprintf(stdout,
		"0x74: MII CS configuration                        0x%04x\n",
		data[0x74 >> 2] & 0xffff);
	if (board_type >= RTL8139_K) {
	fprintf(stdout,
		"0x78: PHY parameter 1                         0x%08x\n"
		"0x7C: Twister parameter                       0x%08x\n",
		data[0x78 >> 2],
		data[0x7C >> 2]);
	if (board_type >= RTL8139A) {
	fprintf(stdout,
		"0x80: PHY parameter 2                               0x%02x\n",
		data8[0x80]);
	}
	}
	if (board_type == RTL8139Cp) {
	fprintf(stdout,
		"0x82: Low addr of a Tx Desc w/ Tx DMA OK          0x%04x\n",
		data[0x80 >> 2] >> 16);
	} else if (board_type == RTL8130) {
	fprintf(stdout,
		"0x82: MII register                                  0x%02x\n",
		data8[0x82]);
	}
	if (board_type >= RTL8139A) {
	fprintf(stdout,
		"0x84: PM CRC for wakeup frame 0                     0x%02x\n"
		"0x85: PM CRC for wakeup frame 1                     0x%02x\n"
		"0x86: PM CRC for wakeup frame 2                     0x%02x\n"
		"0x87: PM CRC for wakeup frame 3                     0x%02x\n"
		"0x88: PM CRC for wakeup frame 4                     0x%02x\n"
		"0x89: PM CRC for wakeup frame 5                     0x%02x\n"
		"0x8A: PM CRC for wakeup frame 6                     0x%02x\n"
		"0x8B: PM CRC for wakeup frame 7                     0x%02x\n",
		data8[0x84],
		data8[0x85],
		data8[0x86],
		data8[0x87],
		data8[0x88],
		data8[0x89],
		data8[0x8A],
		data8[0x8B]);
	fprintf(stdout,
		"0x8C: PM wakeup frame 0            0x%08x 0x%08x\n"
		"0x94: PM wakeup frame 1            0x%08x 0x%08x\n"
		"0x9C: PM wakeup frame 2            0x%08x 0x%08x\n"
		"0xA4: PM wakeup frame 3            0x%08x 0x%08x\n"
		"0xAC: PM wakeup frame 4            0x%08x 0x%08x\n"
		"0xB4: PM wakeup frame 5            0x%08x 0x%08x\n"
		"0xBC: PM wakeup frame 6            0x%08x 0x%08x\n"
		"0xC4: PM wakeup frame 7            0x%08x 0x%08x\n",
		data[0x8C >> 2],
		data[0x90 >> 2],
		data[0x94 >> 2],
		data[0x98 >> 2],
		data[0x9C >> 2],
		data[0xA0 >> 2],
		data[0xA4 >> 2],
		data[0xA8 >> 2],
		data[0xAC >> 2],
		data[0xB0 >> 2],
		data[0xB4 >> 2],
		data[0xB8 >> 2],
		data[0xBC >> 2],
		data[0xC0 >> 2],
		data[0xC4 >> 2],
		data[0xC8 >> 2]);
	fprintf(stdout,
		"0xCC: PM LSB CRC for wakeup frame 0                 0x%02x\n"
		"0xCD: PM LSB CRC for wakeup frame 1                 0x%02x\n"
		"0xCE: PM LSB CRC for wakeup frame 2                 0x%02x\n"
		"0xCF: PM LSB CRC for wakeup frame 3                 0x%02x\n"
		"0xD0: PM LSB CRC for wakeup frame 4                 0x%02x\n"
		"0xD1: PM LSB CRC for wakeup frame 5                 0x%02x\n"
		"0xD2: PM LSB CRC for wakeup frame 6                 0x%02x\n"
		"0xD3: PM LSB CRC for wakeup frame 7                 0x%02x\n",
		data8[0xCC],
		data8[0xCD],
		data8[0xCE],
		data8[0xCF],
		data8[0xD0],
		data8[0xD1],
		data8[0xD2],
		data8[0xD3]);
	}
	if (board_type >= RTL8139B) {
	if (board_type != RTL8100 && board_type != RTL8100B_8139D &&
	    board_type != RTL8101)
	fprintf(stdout,
		"0xD4: Flash memory read/write                 0x%08x\n",
		data[0xD4 >> 2]);
	if (board_type != RTL8130)
	fprintf(stdout,
		"0xD8: Config 5                                      0x%02x\n",
		data8[0xD8]);
	}
	}

	if (board_type == RTL8139Cp || board_type >= RTL8169) {
	v = data[0xE0 >> 2] & 0xffff;
	fprintf(stdout,
		"0xE0: C+ Command                                  0x%04x\n",
		v);
	if (v & (1 << 9))
		fprintf(stdout, "      Big-endian mode\n");
	if (v & (1 << 8))
		fprintf(stdout, "      Home LAN enable\n");
	if (v & (1 << 6))
		fprintf(stdout, "      VLAN de-tagging\n");
	if (v & (1 << 5))
		fprintf(stdout, "      RX checksumming\n");
	if (v & (1 << 4))
		fprintf(stdout, "      PCI 64-bit DAC\n");
	if (v & (1 << 3))
		fprintf(stdout, "      PCI Multiple RW\n");

	v = data[0xe0 >> 2] >> 16;
	fprintf(stdout,
		"0xE2: Interrupt Mitigation                        0x%04x\n"
		"      TxTimer:       %u\n"
		"      TxPackets:     %u\n"
		"      RxTimer:       %u\n"
		"      RxPackets:     %u\n",
		v,
		v >> 12,
		(v >> 8) & 0xf,
		(v >> 4) & 0xf,
		v & 0xf);

	fprintf(stdout,
		"0xE4: Rx Ring Addr                 0x%08x 0x%08x\n",
		data[0xE4 >> 2],
		data[0xE8 >> 2]);

	fprintf(stdout,
		"0xEC: Early Tx threshold                            0x%02x\n",
		data8[0xEC]);

	if (board_type == RTL8139Cp) {
	fprintf(stdout,
		"0xFC: External MII register                   0x%08x\n",
		data[0xFC >> 2]);
	} else if (board_type >= RTL8169 &&
		   board_type != RTL8168_8111Bb &&
		   board_type != RTL8168_8111Bef) {
	fprintf(stdout,
		"0xF0: Func Event                              0x%08x\n"
		"0xF4: Func Event Mask                         0x%08x\n"
		"0xF8: Func Preset State                       0x%08x\n"
		"0xFC: Func Force Event                        0x%08x\n",
		data[0xF0 >> 2],
		data[0xF4 >> 2],
		data[0xF8 >> 2],
		data[0xFC >> 2]);
	}
	}

	return 0;
}
