/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#include "mtlkinc.h"
#include "mtlkcontainer.h"

#define LOG_LOCAL_GID   GID_MTLKCONTAINER
#define LOG_LOCAL_FID   1

#define MTLK_CNT_INITED       0x00000001
#define MTLK_CNT_MUST_ITERATE 0x80000000

#define MTLK_CMP_STARTED      0x00000001

#define MTLK_DEF_ITERATOR_STEP 20 /* ms */

struct mtlk_component
{
  mtlk_dlist_entry_t   lentry;
  mtlk_handle_t        ctx;
  mtlk_component_api_t api;
  char                 name[MTLK_COMPONENT_NAME_LEN + 1];
  uint32               state;
};

int  __MTLK_IFUNC
mtlk_container_init (mtlk_container_t *cont)
{
  memset(cont, 0, sizeof(*cont));

  mtlk_dlist_init(&cont->components);
  cont->state |= MTLK_CNT_INITED;

  return MTLK_ERR_OK;
}

int  __MTLK_IFUNC 
mtlk_container_register (mtlk_container_t       *cont, 
                         const mtlk_component_t *comp)
{
  struct mtlk_component *cmp;

  MTLK_ASSERT(comp != NULL);
  MTLK_ASSERT(comp->api != NULL);

  cmp = (struct mtlk_component *)mtlk_osal_mem_alloc(sizeof(*cmp),
                                                     MTLK_MEM_TAG_CONTAINER);
  if (!cmp) {
    ELOG_V("Can't allocate component entry");
    return  MTLK_ERR_NO_MEM;
  }

  memset(cmp, 0, sizeof(*cmp));
  
  cmp->api = *comp->api;

  wave_strcopy(cmp->name, comp->name, sizeof(cmp->name));

  if (cmp->api.iterate) {
    /* Push components with iterators to the begin of the list */
    mtlk_dlist_push_front(&cont->components, &cmp->lentry);
    cont->state |= MTLK_CNT_MUST_ITERATE;
  }
  else {
    mtlk_dlist_push_back(&cont->components, &cmp->lentry);
  }

  return MTLK_ERR_OK;
}

int  __MTLK_IFUNC
mtlk_container_run (mtlk_container_t  *cont,
                    mtlk_osal_event_t *close_evt)
{
  int                    res = MTLK_ERR_UNKNOWN;
  mtlk_dlist_entry_t    *head;
  mtlk_dlist_entry_t    *entry;
  struct mtlk_component *cmp;
  uint32                 step_ms;

  /* Traverse the components list and Start all the components */
  mtlk_dlist_foreach(&cont->components, entry, head) {
    cmp = MTLK_CONTAINER_OF(entry, struct mtlk_component, lentry);

    /* Start component if required */
    if (cmp->api.start) {
      cmp->ctx = cmp->api.start();
      if (cmp->ctx == HANDLE_T(0)) {
        ELOG_S("Can't start component: '%s'", cmp->name);
        goto end;
      }
    }
    cmp->state |= MTLK_CMP_STARTED;
  }

  step_ms = (cont->state & MTLK_CNT_MUST_ITERATE)?MTLK_DEF_ITERATOR_STEP:MTLK_OSAL_EVENT_INFINITE;
  while (TRUE) {
    res = mtlk_osal_event_wait(close_evt, step_ms);

    if (res == MTLK_ERR_OK) {
      /* Stop event signaled => exit loop */
      break;
    }

    mtlk_dlist_foreach(&cont->components, entry, head) {
      cmp = MTLK_CONTAINER_OF(entry, struct mtlk_component, lentry);
      if (cmp->api.iterate) {
        uint32 req_step = cmp->api.iterate(cmp->ctx);
        MTLK_ASSERT(req_step != 0);

        step_ms = MIN(req_step, step_ms);
      }
      else {
        /* all the components with iterators inserted to the begin of the list */
        break;
      }
    }
  }

end:
  ILOG1_V("Stop signaled...");
  /* Traverse the components list and Stop all the components */
  mtlk_dlist_foreach(&cont->components, entry, head) {
    cmp = MTLK_CONTAINER_OF(entry, struct mtlk_component, lentry);

    /* Stop the component if required */
    if ((MTLK_CMP_STARTED & cmp->state) && cmp->api.stop) {
      ILOG1_S("Trying to stop component %s...", cmp->name);
      cmp->api.stop(cmp->ctx);
      ILOG1_S("Done! (%s)", cmp->name);
    }
    cmp->state &= ~MTLK_CMP_STARTED;
  }

  return res;
}

void __MTLK_IFUNC
mtlk_container_cleanup (mtlk_container_t *cont)
{
  mtlk_dlist_entry_t *le;

  MTLK_ASSERT((cont->state & MTLK_CNT_INITED) != 0);
  
  while ((le = mtlk_dlist_pop_front(&cont->components)) != NULL) {
    struct mtlk_component *cmp = MTLK_CONTAINER_OF(le, struct mtlk_component, lentry);
    mtlk_osal_mem_free(cmp);
  }

  mtlk_dlist_cleanup(&cont->components);
}
