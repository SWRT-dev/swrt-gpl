/* api_driver97_init.h
 *
 * SafeXcel-IP-97 Driver Initialization Interface
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

#ifndef DRIVER97_INIT_H_
#define DRIVER97_INIT_H_

#include <linux/platform_device.h>

/*----------------------------------------------------------------------------
 * Driver97_Init
 *
 * Initialize the driver. This function must be called before any other
 * driver API function can be called.
 *
 * Returns 0 for success and -1 for failure.
 */
int
Driver97_Init(struct platform_device *pdev);

/*----------------------------------------------------------------------------
 * Driver97_Exit
 *
 * Initialize the driver. After this function is called no other driver API
 * function can be called except Driver97_Init().
 */
void
Driver97_Exit(void);


#endif /* DRIVER97_INIT_H_ */


/* end of file api_driver97_init.h */
