/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
/* $Id$ */

#ifndef __HW_MTLK_CARD_SELECTOR_H__
#define __HW_MTLK_CARD_SELECTOR_H__

#ifdef MTLK_DEBUG

static __INLINE BOOL
_known_card_type(mtlk_card_type_t x)
{
  return (x > MTLK_CARD_FIRST) && (x < MTLK_CARD_LAST);
}

#endif /* MTLK_DEBUG */

#define CARD_SELECTOR_START(hw_type) \
  for(;;) { \
    MTLK_ASSERT(_known_card_type(hw_type)); \
    switch(hw_type) \
    { \
    default: \
      MTLK_ASSERT(!"Actions for all known hw types to be explicitly provided in selector");

#define CARD_SELECTOR_END() \
    } \
    break; \
  }

#define IF_CARD_PCIEG6(op) \
    case MTLK_CARD_PCIEG6: {op;} break

#define __CASE_PCIEG6 case MTLK_CARD_PCIEG6:

#define IF_CARD_PCI_PCIE(op)   \
          __CASE_PCIEG6        \
              {op;} break

/*Gen6 on PCIe */
#define IF_CARD_G6(op)  \
      __CASE_PCIEG6     \
          {op;} break


#endif /* __HW_MTLK_CARD_SELECTOR_H__ */
