/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
#ifndef __WPS_CTRL_H__
#define __WPS_CTRL_H__

#include "mtlkcontainer.h"


extern const mtlk_component_api_t irb_wps_ctrl_api;

/* Private API.
 */
void wps_ctrl_set_leds_resolution(int val);
int  wps_ctrl_set_wps_script_path(const char* str);

#endif /* __WPS_CTRL_H__ */
