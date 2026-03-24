/* cs_adapter_ext.h
 *
 * Configuration Settings for the SLAD Adapter Combined module,
 * extensions.
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

#ifndef CS_ADAPTER_EXT_H_
#define CS_ADAPTER_EXT_H_


/****************************************************************************
 * Adapter Global configuration parameters
 */


/****************************************************************************
 * Adapter PEC configuration parameters
 */


/****************************************************************************
 * Adapter EIP-202 configuration parameters
 */

// Enables the EIP-207 Record Cache interface for the record invalidation
//#define ADAPTER_EIP202_RC_SUPPORT

#ifdef DRIVER_DMARESOURCE_BANKS_ENABLE
#define ADAPTER_EIP202_DMARESOURCE_BANKS_ENABLE

#define ADAPTER_EIP202_TRANSFORM_RECORD_COUNT      \
                                        DRIVER_TRANSFORM_RECORD_COUNT
#define ADAPTER_EIP202_TRANSFORM_RECORD_BYTE_COUNT \
                                        DRIVER_TRANSFORM_RECORD_BYTE_COUNT
#endif // DRIVER_DMARESOURCE_BANKS_ENABLE


#endif // CS_ADAPTER_EXT_H_


/* end of file cs_adapter_ext.h */
