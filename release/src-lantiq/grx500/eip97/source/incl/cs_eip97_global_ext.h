/* cs_eip97_global_ext.h
 *
 * Top-level configuration parameters extensions
 * for the EIP-97 Global Control Driver Library
 *
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

#ifndef CS_EIP97_GLOBAL_EXT_H_
#define CS_EIP97_GLOBAL_EXT_H_



/*----------------------------------------------------------------------------
 * Definitions and macros
 */

// Define this parameter in order to configure the DFE and DSE ring priorities
#define EIP97_GLOBAL_DFE_DSE_PRIO_CONFIGURE

// EIP-207s Classification Support, DMA Control base address
#define EIP97_RC_BASE      0x37000

// EIP-207s Classification Support, DMA Control base address
#define EIP97_BASE         0x1FFF4


#endif /* CS_EIP97_GLOBAL_EXT_H_ */


/* end of file cs_eip97_global_ext.h */
