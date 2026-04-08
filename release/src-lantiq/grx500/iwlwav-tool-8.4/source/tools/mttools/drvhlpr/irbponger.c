/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
#include "mtlkinc.h"
#include "irbponger.h"
#include "dataex.h"
#include "mtlkirba.h"

#define LOG_LOCAL_GID   GID_IRB
#define LOG_LOCAL_FID   2

static const mtlk_guid_t IRBE_PING = MTLK_IRB_GUID_PING;
static const mtlk_guid_t IRBE_PONG = MTLK_IRB_GUID_PONG;

static void __MTLK_IFUNC
_irb_ping_handler (mtlk_irba_t       *irba,
                   mtlk_handle_t      context,
                   const mtlk_guid_t *evt,
                   void              *buffer,
                   uint32            size)
{
  MTLK_UNREFERENCED_PARAM(context);
  MTLK_ASSERT(mtlk_guid_compare(&IRBE_PING, evt) == 0);

  mtlk_irba_call_drv(MTLK_IRBA_ROOT, &IRBE_PONG, buffer, size);
}


static mtlk_handle_t
irb_ponger_start (void)
{
  mtlk_irba_handle_t *h = mtlk_irba_register(MTLK_IRBA_ROOT, &IRBE_PING, 1, _irb_ping_handler, 0);
  return HANDLE_T(h);
}

static void
irb_ponger_stop (mtlk_handle_t ctx)
{
  mtlk_irba_unregister(MTLK_IRBA_ROOT, HANDLE_T_PTR(mtlk_irba_handle_t, ctx));
}

const mtlk_component_api_t
irb_ponger_component_api = {
  irb_ponger_start,
  NULL,
  irb_ponger_stop
};
