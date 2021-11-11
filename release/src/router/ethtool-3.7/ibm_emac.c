/*
 *  Copyright (c) 2004, 2005 Zultys Technologies
 *  Eugene Surovegin <eugene.surovegin@zultys.com> or <ebs@ebshome.net>
 */
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

#include "internal.h"

/* Ethtool get_regs complex data.
 * we want to get not just EMAC registers, but also MAL, ZMII, RGMII, TAH
 * when available.
 *
 * Returned BLOB consists of the ibm_emac_ethtool_regs_hdr,
 * MAL registers, EMAC registers and optional ZMII, RGMII, TAH registers.
 * Each register component is preceded with emac_ethtool_regs_subhdr.
 * Order of the optional headers follows their relative bit posititions
 * in emac_ethtool_regs_hdr.components
 */
#define EMAC_ETHTOOL_REGS_ZMII		0x00000001
#define EMAC_ETHTOOL_REGS_RGMII		0x00000002
#define EMAC_ETHTOOL_REGS_TAH		0x00000004

struct emac_ethtool_regs_hdr {
	u32 components;
};

struct emac_ethtool_regs_subhdr {
	u32 version;
	u32 index;
};

struct emac_regs {
	u32 mr0;
	u32 mr1;
	u32 tmr0;
	u32 tmr1;
	u32 rmr;
	u32 isr;
	u32 iser;
	u32 iahr;
	u32 ialr;
	u32 vtpid;
	u32 vtci;
	u32 ptr;
	u32 iaht1;
	u32 iaht2;
	u32 iaht3;
	u32 iaht4;
	u32 gaht1;
	u32 gaht2;
	u32 gaht3;
	u32 gaht4;
	u32 lsah;
	u32 lsal;
	u32 ipgvr;
	u32 stacr;
	u32 trtr;
	u32 rwmr;
	u32 octx;
	u32 ocrx;
	u32 ipcr;
};

struct mal_regs {
	u32 tx_count;
	u32 rx_count;

	u32 cfg;
	u32 esr;
	u32 ier;
	u32 tx_casr;
	u32 tx_carr;
	u32 tx_eobisr;
	u32 tx_deir;
	u32 rx_casr;
	u32 rx_carr;
	u32 rx_eobisr;
	u32 rx_deir;
	u32 tx_ctpr[32];
	u32 rx_ctpr[32];
	u32 rcbs[32];
};

struct zmii_regs {
	u32 fer;
	u32 ssr;
	u32 smiisr;
};

struct rgmii_regs {
	u32 fer;
	u32 ssr;
};

struct tah_regs {
	u32 revid;
	u32 pad[3];
	u32 mr;
	u32 ssr0;
	u32 ssr1;
	u32 ssr2;
	u32 ssr3;
	u32 ssr4;
	u32 ssr5;
	u32 tsr;
};

static void *print_emac_regs(void *buf)
{
	struct emac_ethtool_regs_subhdr *hdr = buf;
	struct emac_regs *p = (struct emac_regs *)(hdr + 1);
	void *res = p + 1;

	printf("EMAC%d Registers\n", hdr->index);
	printf("-----------------\n");
	printf("MR0   = 0x%08x MR1  = 0x%08x RMR = 0x%08x\n"
	       "ISR   = 0x%08x ISER = 0x%08x\n"
	       "TMR0  = 0x%08x TMR1 = 0x%08x\n"
	       "TRTR  = 0x%08x RWMR = 0x%08x\n"
	       "IAR   = %04x%08x\n"
	       "LSA   = %04x%08x\n"
	       "IAHT  = 0x%04x 0x%04x 0x%04x 0x%04x\n"
	       "GAHT  = 0x%04x 0x%04x 0x%04x 0x%04x\n"
	       "VTPID = 0x%04x VTCI = 0x%04x\n"
	       "IPGVR = 0x%04x STACR = 0x%08x\n"
	       "OCTX  = 0x%08x OCRX = 0x%08x\n",
	       p->mr0, p->mr1, p->rmr,
	       p->isr, p->iser,
	       p->tmr0, p->tmr1,
	       p->trtr, p->rwmr,
	       p->iahr, p->ialr,
	       p->lsah, p->lsal,
	       p->iaht1, p->iaht2, p->iaht3, p->iaht4,
	       p->gaht1, p->gaht2, p->gaht3, p->gaht4,
	       p->vtpid, p->vtci, p->ipgvr, p->stacr, p->octx, p->ocrx);

	if (hdr->version)
		printf(" IPCR = 0x%08x\n\n", p->ipcr);
	else {
		printf("\n\n");
		res -= sizeof(u32);
	}
	return res;
}

static void *print_mal_regs(void *buf)
{
	struct emac_ethtool_regs_subhdr *hdr = buf;
	struct mal_regs *p = (struct mal_regs *)(hdr + 1);
	int i;

	printf("MAL%d Registers\n", hdr->index);
	printf("-----------------\n");
	printf("CFG = 0x%08x ESR = 0x%08x IER = 0x%08x\n"
	       "TX|CASR = 0x%08x CARR = 0x%08x EOBISR = 0x%08x DEIR = 0x%08x\n"
	       "RX|CASR = 0x%08x CARR = 0x%08x EOBISR = 0x%08x DEIR = 0x%08x\n",
	       p->cfg, p->esr, p->ier,
	       p->tx_casr, p->tx_carr, p->tx_eobisr, p->tx_deir,
	       p->rx_casr, p->rx_carr, p->rx_eobisr, p->rx_deir);

	printf("TX|");
	for (i = 0; i < p->tx_count; ++i) {
		if (i && !(i % 4))
			printf("\n   ");
		printf("CTP%d = 0x%08x ", i, p->tx_ctpr[i]);
	}
	printf("\nRX|");
	for (i = 0; i < p->rx_count; ++i) {
		if (i && !(i % 4))
			printf("\n   ");
		printf("CTP%d = 0x%08x ", i, p->rx_ctpr[i]);
	}
	printf("\n   ");
	for (i = 0; i < p->rx_count; ++i) {
		u32 r = p->rcbs[i];
		if (i && !(i % 3))
			printf("\n   ");
		printf("RCBS%d = 0x%08x (%d) ", i, r, r * 16);
	}
	printf("\n\n");
	return p + 1;
}

static void *print_zmii_regs(void *buf)
{
	struct emac_ethtool_regs_subhdr *hdr = buf;
	struct zmii_regs *p = (struct zmii_regs *)(hdr + 1);

	printf("ZMII%d Registers\n", hdr->index);
	printf("-----------------\n");
	printf("FER    = %08x SSR = %08x\n"
	       "SMIISR = %08x\n\n", p->fer, p->ssr, p->smiisr);

	return p + 1;
}

static void *print_rgmii_regs(void *buf)
{
	struct emac_ethtool_regs_subhdr *hdr = buf;
	struct rgmii_regs *p = (struct rgmii_regs *)(hdr + 1);

	printf("RGMII%d Registers\n", hdr->index);
	printf("-----------------\n");
	printf("FER    = %08x SSR = %08x\n\n", p->fer, p->ssr);

	return p + 1;
}

static void *print_tah_regs(void *buf)
{
	struct emac_ethtool_regs_subhdr *hdr = buf;
	struct tah_regs *p = (struct tah_regs *)(hdr + 1);

	printf("TAH%d Registers\n", hdr->index);
	printf("-----------------\n");

	printf("REVID = %08x MR = %08x TSR = %08x\n"
	       "SSR0  = %08x SSR1 = %08x SSR2 = %08x\n"
	       "SSR3  = %08x SSR4 = %08x SSR5 = %08x\n\n",
	       p->revid, p->mr, p->tsr,
	       p->ssr0, p->ssr1, p->ssr2, p->ssr3, p->ssr4, p->ssr5);

	return p + 1;
}

int ibm_emac_dump_regs(struct ethtool_drvinfo *info, struct ethtool_regs *regs)
{
	struct emac_ethtool_regs_hdr *hdr =
	    (struct emac_ethtool_regs_hdr *)regs->data;
	void *buf = hdr + 1;

	buf = print_mal_regs(buf);
	buf = print_emac_regs(buf);
	if (hdr->components & EMAC_ETHTOOL_REGS_ZMII)
		buf = print_zmii_regs(buf);
	if (hdr->components & EMAC_ETHTOOL_REGS_RGMII)
		buf = print_rgmii_regs(buf);
	if (hdr->components & EMAC_ETHTOOL_REGS_TAH)
		print_tah_regs(buf);

	return 0;
}
