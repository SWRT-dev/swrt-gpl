/* eip206_level0_ext.h
 *
 * EIP-206 Processing Engine Level0 Internal interface extensions
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

#ifndef EIP206_LEVEL0_EXT_H_
#define EIP206_LEVEL0_EXT_H_


/*----------------------------------------------------------------------------
 * This module uses (requires) the following interface(s):
 */

// Driver Framework Basic Definitions API
#include "basic_defs.h"         // IDENTIFIER_NOT_USED, bool

// Driver Framework Device API
#include "device_types.h"       // Device_Handle_t


/*----------------------------------------------------------------------------
 * Definitions and macros
 */


/*----------------------------------------------------------------------------
 * Local variables
 */


static inline void
EIP206_ARC4_SIZE_WR(
        Device_Handle_t Device,
        const unsigned int PEnr,
        const bool fLarge)
{
    IDENTIFIER_NOT_USED(Device);
    IDENTIFIER_NOT_USED(PEnr);
    IDENTIFIER_NOT_USED(fLarge);

    return; // Not supported on this engine, stub implementation
}


#endif /* EIP206_LEVEL0_EXT_H_ */


/* end of file eip206_level0_ext.h */
