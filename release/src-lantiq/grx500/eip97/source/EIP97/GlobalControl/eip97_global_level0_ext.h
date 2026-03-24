/* eip97_global_level0_ext.h
 *
 * EIP-97 Global Control Level0 Internal interface extensions
 */

/*****************************************************************************
* Copyright (c) 2011-2013 INSIDE Secure B.V. All Rights Reserved.
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

#ifndef EIP97_GLOBAL_LEVEL0_EXT_H_
#define EIP97_GLOBAL_LEVEL0_EXT_H_


/*----------------------------------------------------------------------------
 * This module implements (provides) the following interface(s):
 */



/*----------------------------------------------------------------------------
 * This module uses (requires) the following interface(s):
 */

// Driver Framework Basic Definitions API
#include "basic_defs.h"         // BIT definitions, bool, uint32_t

// Driver Framework Device API
#include "device_types.h"       // Device_Handle_t


/*----------------------------------------------------------------------------
 * Definitions and macros
 */


/*----------------------------------------------------------------------------
 * Local variables
 */


static inline void
EIP97_OPTIONS_RD(
        Device_Handle_t Device,
        uint8_t * const NofPes,
        uint8_t * const InTbufSize,
        uint8_t * const InDbufSize,
        uint8_t * const OutTbufSize,
        uint8_t * const OutDbufSize,
        bool * const fTG,
        bool * const fTRC)
{
    uint32_t RevRegVal;

    RevRegVal = EIP97_Read32(Device, EIP97_REG_OPTIONS);

    *fTRC         = ((RevRegVal & BIT_31) != 0);
    *fTG          = ((RevRegVal & BIT_30) != 0);
    *OutDbufSize  = (uint8_t)((RevRegVal >> 16) & MASK_4_BITS);
    *OutTbufSize  = (uint8_t)((RevRegVal >> 12) & MASK_4_BITS);
    *InDbufSize   = (uint8_t)((RevRegVal >> 8)  & MASK_4_BITS);
    *InTbufSize   = (uint8_t)((RevRegVal >> 4)  & MASK_4_BITS);
    *NofPes       = (uint8_t)((RevRegVal)       & MASK_4_BITS);
}


#endif /* EIP97_GLOBAL_LEVEL0_EXT_H_ */


/* end of file eip97_global_level0_ext.h */



