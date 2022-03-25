/* This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __LINUX_BRIDGE_EBT_EXTMARK_T_H
#define __LINUX_BRIDGE_EBT_EXTMARK_T_H

/* The target member is reused for adding new actions, the
 * value of the real target is -1 to -NUM_STANDARD_TARGETS.
 * For backward compatibility, the 4 lsb (2 would be enough,
 * but let's play it safe) are kept to designate this target.
 * The remaining bits designate the action. By making the set
 * action 0xfffffff0, the result will look ok for older
 * versions. [September 2006]
 */

#define EXTMARK_SET_VALUE (0xfffffff0)
#define EXTMARK_OR_VALUE  (0xffffffe0)
#define EXTMARK_AND_VALUE (0xffffffd0)
#define EXTMARK_XOR_VALUE (0xffffffc0)

struct ebt_extmark_t_info {
	unsigned long extmark;
	/* EBT_ACCEPT, EBT_DROP, EBT_CONTINUE or EBT_RETURN */
	int target;
};
#define EBT_EXTMARK_TARGET "extmark"

#endif
