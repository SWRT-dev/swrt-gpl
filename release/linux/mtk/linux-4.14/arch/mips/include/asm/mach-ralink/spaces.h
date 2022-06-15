
#ifndef _ASM_RALINK_SPACES_H
#define _ASM_RALINK_SPACES_H

/*
 * Memory Map for MT7621
 *
 * 0x00000000 - 0x1bffffff: 1st RAM region, 448MB
 * 0x20000000 - 0x23ffffff: 2st RAM region, 64MB
 * 
 */

#define HIGHMEM_START	_AC(0x20000000, UL)

#include <asm/mach-generic/spaces.h>

#endif /* _ASM_RALINK_SPACES_H */

