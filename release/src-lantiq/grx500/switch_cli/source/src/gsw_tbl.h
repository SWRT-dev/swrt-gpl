/****************************************************************************

                               Copyright 2010
                          Lantiq Deutschland GmbH
                   Am Campeon 3; 85579 Neubiberg, Germany

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*****************************************************************************/
#ifndef _INCLUDE_GSW_TBL_H
#define _INCLUDE_GSW_TBL_H

#include "ltq_cli_lib.h"
#ifndef IOCTL_PCTOOL
#include <stdio.h>
#endif

extern const GSW_CommandTable_t cmdTable[];
extern const GSW_CommandTable_t cmdTableMacsec[];


//#ifdef SWITCHAPI_HELP_TEXT
extern const char *HelpText[];
//#endif /* SWITCHAPI_HELP_TEXT */

#endif /* _INCLUDE_GSW_TBL_H */
