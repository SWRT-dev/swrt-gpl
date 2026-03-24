/* eip202_global_hw_interface.h
 *
 * EIP-202 HIA Global Control HW Internal interface
 */

/*****************************************************************************
* Copyright (c) 2012-2013 INSIDE Secure B.V. All Rights Reserved.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 2 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************/

#ifndef EIP202_GLOBAL_HW_INTERFACE_H_
#define EIP202_GLOBAL_HW_INTERFACE_H_


/*----------------------------------------------------------------------------
 * This module uses (requires) the following interface(s):
 */

// Default configuration
#include "c_eip202_global.h"

// Driver Framework Basic Definitions API
#include "basic_defs.h"         // uint16_t


/*----------------------------------------------------------------------------
 * Definitions and macros
 */

#define EIP202_DFE_TRD_REG_STAT_IDLE    0xF

// Read/Write register constants

/*****************************************************************************
 * Byte offsets of the EIP-202 HIA registers
 *****************************************************************************/
// EIP-202 HIA EIP number (0xCA) and complement (0x35)
#define EIP202_SIGNATURE          ((uint16_t)0x35CA)

#define EIP202_REG_OFFS           4
#define EIP202_FE_REG_MAP_SIZE    64

// HIA DFE all threads
#define EIP202_DFE_REG_CFG        ((EIP202_DFE_BASE)+(0x00 * EIP202_REG_OFFS))
#define EIP202_DFE_REG_PRIO_0     ((EIP202_DFE_BASE)+(0x04 * EIP202_REG_OFFS))
#define EIP202_DFE_REG_PRIO_1     ((EIP202_DFE_BASE)+(0x05 * EIP202_REG_OFFS))
#define EIP202_DFE_REG_PRIO_2     ((EIP202_DFE_BASE)+(0x06 * EIP202_REG_OFFS))
#define EIP202_DFE_REG_PRIO_3     ((EIP202_DFE_BASE)+(0x07 * EIP202_REG_OFFS))

// HIA DFE thread n (n - number of the DFE thread)
#define EIP202_DFE_TRD_REG_CTRL(n)   ((EIP202_FE_REG_MAP_SIZE * n) + \
                                      ((EIP202_DFE_TRD_BASE) + \
                                       (0x00 * EIP202_REG_OFFS)))
#define EIP202_DFE_TRD_REG_STAT(n)   ((EIP202_FE_REG_MAP_SIZE * n) + \
                                      ((EIP202_DFE_TRD_BASE) + \
                                       (0x01 * EIP202_REG_OFFS)))

// HIA DSE all threads
#define EIP202_DSE_REG_CFG        ((EIP202_DSE_BASE)+(0x00 * EIP202_REG_OFFS))
#define EIP202_DSE_REG_PRIO_0     ((EIP202_DSE_BASE)+(0x04 * EIP202_REG_OFFS))
#define EIP202_DSE_REG_PRIO_1     ((EIP202_DSE_BASE)+(0x05 * EIP202_REG_OFFS))
#define EIP202_DSE_REG_PRIO_2     ((EIP202_DSE_BASE)+(0x06 * EIP202_REG_OFFS))
#define EIP202_DSE_REG_PRIO_3     ((EIP202_DSE_BASE)+(0x07 * EIP202_REG_OFFS))

// HIA DSE thread n (n - number of the DSE thread)
#define EIP202_DSE_TRD_REG_CTRL(n)   ((EIP202_FE_REG_MAP_SIZE * n) + \
                                      ((EIP202_DSE_TRD_BASE) + \
                                       (0x00 * EIP202_REG_OFFS)))
#define EIP202_DSE_TRD_REG_STAT(n)   ((EIP202_FE_REG_MAP_SIZE * n) + \
                                      ((EIP202_DSE_TRD_BASE) + \
                                       (0x01 * EIP202_REG_OFFS)))

// HIA Global
#define EIP202_G_REG_MST_CTRL     ((EIP202_G_BASE)+(0x00 * EIP202_REG_OFFS))
#define EIP202_G_REG_OPTIONS      ((EIP202_G_BASE)+(0x01 * EIP202_REG_OFFS))
#define EIP202_G_REG_VERSION      ((EIP202_G_BASE)+(0x02 * EIP202_REG_OFFS))

// Default EIP202_DFE_REG_CFG register value
#define EIP202_DFE_REG_CFG_DEFAULT          0x00000000

// Default EIP202_DFE_TRD_REG_CTRL register value
#define EIP202_DFE_TRD_REG_CTRL_DEFAULT     0x00000000

// Default EIP202_DSE_REG_CFG register value
#define EIP202_DSE_REG_CFG_DEFAULT          0x00008000

// Default EIP202_DSE_TRD_REG_CTRL register value
#define EIP202_DSE_TRD_REG_CTRL_DEFAULT     0x00000000

// Default EIP202_DSE_REG_PRIO_x register values
#define EIP202_DSE_REG_PRIO_0_DEFAULT       0x00000000
#define EIP202_DSE_REG_PRIO_1_DEFAULT       0x00000000
#define EIP202_DSE_REG_PRIO_2_DEFAULT       0x00000000
#define EIP202_DSE_REG_PRIO_3_DEFAULT       0x00000000

// Default EIP202_DFE_REG_PRIO_x register values
#define EIP202_DFE_REG_PRIO_0_DEFAULT       0x00000000
#define EIP202_DFE_REG_PRIO_1_DEFAULT       0x00000000
#define EIP202_DFE_REG_PRIO_2_DEFAULT       0x00000000
#define EIP202_DFE_REG_PRIO_3_DEFAULT       0x00000000


#endif /* EIP202_GLOBAL_HW_INTERFACE_H_ */


/* end of file eip202_global_hw_interface.h */
