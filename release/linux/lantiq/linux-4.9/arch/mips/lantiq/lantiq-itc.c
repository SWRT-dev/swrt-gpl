/*
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 *  Copyright (C) 2009~2015 Lantiq Deutschland GmbH
 *  Copyright (C) 2016 Intel Corporation.
 */

#include <linux/init.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/cpu.h>
#include <linux/sched.h>
#include <asm/mipsregs.h>
#include <asm/ltq_itc.h>


uint32_t		*ITC_base;
uint32_t		*ITC_Sem_base;
uint32_t		*ITC_FIFO_base;
uint32_t	itcn;

int32_t itc_init(void)
{
	uint32_t	errctlreg, Config_ITC;
	uint32_t	ITCAddressMap0, ITCAddressMap1;
	uint32_t	*ITC_BlockNC;
	uint32_t	*ITC_Cell_Sem;
	uint32_t	*ITC_Cell_FIFO;
	uint32_t	ITC_Cell_Sem_off;
	uint32_t	i;

/* Configure ITC Tags using Cache opts .*/
/* Set ITC bit in ErrCtl register to enable Cache mode for ITC Tags */

	errctlreg = read_c0_ecc();
	Config_ITC = errctlreg | ERRCTL_ITC;
	write_c0_ecc(Config_ITC);
	mips_ihb();

		/* Read reset-value ITC_Address_Map0 */
	__asm__ volatile
		("\
			cache 5, 0($0);  \
			ehb; \
			"
		);

	if (read_c0_dtaglo() & ITC_En) {
		pr_info("ITC Memory is already initialised for Core %d at address %x !!!",
			(smp_processor_id()/2), (read_c0_dtaglo() & 0xffff0000));

		/* return ErrCtl to it previous state */
			write_c0_ecc(errctlreg);
		mips_ihb();

		return 0;
	}

#ifdef DEBUG_ITC
		/* Read reset-value ITC_Address_Map1 */
	__asm__ volatile
		("\
			cache 5, 8($0);  \
			ehb; \
			"
		);
	pr_info("\ndef ITC_Address_Map1 %08x", read_c0_dtaglo());
#endif

/*
 *configure Number of entries Address mask bits and
 * Entry Grain in ITC tag index 8
 */
	ITCAddressMap1 = ((ITC_AddrMask << 10) | ITC_EntryGrain);

	write_c0_dtaglo(ITCAddressMap1);

	__asm__ volatile
		("\
			cache 9, 8($0);  \
			ehb; \
			"
		);

#ifdef DEBUG_ITC
		/* Read new-value ITC_Address_Map1 */
	__asm__ volatile
		("\
			cache 5, 8($0);  \
			ehb; \
			"
		);
	pr_info("\nnew ITC_Address_Map1 %08x", read_c0_dtaglo());
#endif

/*
 *configure Base address and ITC_En (enable bit) in
 *  ITC tag index 0 and Use physical address
 */
	ITC_BlockNC = (unsigned int *)((unsigned int)ITC_Block & 0x7fffffff);

#ifdef DEBUG_ITC
		/* Read reset-value ITC_Address_Map0 */
	__asm__ volatile
		("\
			cache 5, 0($0);  \
			ehb; \
			"
		);
	pr_info("\ndef ITC_Address_Map0 %08x", read_c0_dtaglo());
#endif

	ITCAddressMap0 = ((unsigned int)ITC_BlockNC | ITC_En);
	write_c0_dtaglo(ITCAddressMap0);

	__asm__ volatile
		("\
			cache 9, 0($0); \
			ehb; \
			"
		);

#ifdef DEBUG_ITC
		/* Read new-value ITC_Address_Map0 */
	__asm__ volatile
		("\
			cache 5, 0($0); \
			ehb; \
			"
		);
	pr_info("\nnew ITC_Address_Map0 %08x", read_c0_dtaglo());
#endif

/* return ErrCtl to it previous state */
	write_c0_ecc(errctlreg);
	mips_ihb();

/* Enable ITC Entry :  Use unmapped address */
	ITC_BlockNC = (unsigned int *)((unsigned int)ITC_Block);

/* Change to unmapped memory */
	ITC_BlockNC = (unsigned int *)CKSEG1ADDR(ITC_BlockNC);

	ITC_base = (unsigned int *)((unsigned int)ITC_BlockNC);
	ITC_FIFO_base = (unsigned int *)((unsigned int)ITC_BlockNC);

	ITC_Cell_Sem_off = 0;

/*Use Control View to access Entry Tag*/

	for (i = 0; i < ITC_FIFO_Entries; i++) {
		ITC_Cell_FIFO = (unsigned int *)(((unsigned int)ITC_BlockNC + ITC_Cell_Sem_off) | ITC_BypassView);
		*ITC_Cell_FIFO = 0;

		ITC_Cell_FIFO = (unsigned int *)(((unsigned int)ITC_BlockNC + ITC_Cell_Sem_off) | ITC_ControlView);
		*ITC_Cell_FIFO = ITC_E;

		ITC_Cell_Sem_off = ITC_Cell_Sem_off + (128 * (0x1 << ITC_EntryGrain));
	}

	ITC_Sem_base = (unsigned int *)((unsigned int)ITC_BlockNC + ITC_Cell_Sem_off) ;

/*
 * For each SEM entries clear the content of Cell using ITC_BypassView
 * otherwise default value seen in cell is 5.
 * Set the ITC_ControlView to set ITC_E.
 * Use ITC_PVSyncView to init the Sem Cell to 1 to unblock the first access
 */

	for (i = 0; i < ITC_SEM_Entries; i++) {
		ITC_Cell_Sem = (unsigned int *)(((unsigned int)ITC_BlockNC + ITC_Cell_Sem_off) | ITC_BypassView);
		*ITC_Cell_Sem = 0;

		ITC_Cell_Sem = (unsigned int *)(((unsigned int)ITC_BlockNC + ITC_Cell_Sem_off) | ITC_ControlView);
		*ITC_Cell_Sem = ITC_E;

		ITC_Cell_Sem = (unsigned int *)(((unsigned int)ITC_BlockNC + ITC_Cell_Sem_off) | ITC_PVSyncView);
		*ITC_Cell_Sem = 1;

		ITC_Cell_Sem_off = ITC_Cell_Sem_off + (128 * (0x1 << ITC_EntryGrain));
	}

	return 0;
}
early_initcall(itc_init);


/*
 * Read the ITC cell using P/V Sync View,
 *If the Cell Contains 0, this will block
 */

void itc_sem_wait(uint8_t semId)
{
	uint32_t *ITC_Cell;
	uint32_t Sem_off = 0;

	Sem_off = semId * (128 * (0x1 << ITC_EntryGrain));

	ITC_Cell = (uint32_t *)(((uint32_t)ITC_Sem_base + Sem_off) | ITC_PVSyncView);

	itcn = *ITC_Cell;
}
EXPORT_SYMBOL(itc_sem_wait);

/*
 *Write to the ITC cell, to increment its value,
 * This will unblock the lock.
 */

void itc_sem_post(uint8_t semId)
{
	uint32_t *ITC_Cell;
	uint32_t Sem_off = 0;

	Sem_off = semId * (128 * (0x1 << ITC_EntryGrain));

	ITC_Cell = (uint32_t *)(((uint32_t)ITC_Sem_base + Sem_off) | ITC_PVSyncView);

	*ITC_Cell = 1;
}
EXPORT_SYMBOL(itc_sem_post);

uint32_t itc_sem_addr(uint8_t semId)
{ /*copy from itc_sem_wait */
	uint32_t Sem_off = 0;

	Sem_off = semId * (128 * (0x1 << ITC_EntryGrain));

	return ((uint32_t)ITC_Sem_base + Sem_off) | ITC_PVSyncView;
}
EXPORT_SYMBOL(itc_sem_addr);




