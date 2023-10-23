/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

/* $Id$ */

#ifndef __HW_MTLK_CARD_SELECTOR_H__
#define __HW_MTLK_CARD_SELECTOR_H__

#define CARD_SELECTOR_START(card_type) \
  for(;;) { \
    MTLK_ASSERT(wave_known_card_type(card_type)); \
    switch(card_type) \
    { \
    default: \
      MTLK_ASSERT(!"Actions for all known card types to be explicitly provided in selector"); \
      FALLTHROUGH;

#define CARD_SELECTOR_END() \
    } \
    break; \
  }

/* Gen6 on PCIe */
#define __CASE_PCIEG6 \
    case MTLK_CARD_PCIEG6_A0_B0: \
    case MTLK_CARD_PCIEG6_D2:

#define IF_CARD_PCIEG6(op) \
    __CASE_PCIEG6 \
        {op;} break

#define IF_CARD_PCIEG6_A0_B0(op) \
    case MTLK_CARD_PCIEG6_A0_B0: \
        {op;} break

#define IF_CARD_PCIEG6_D2(op) \
    case MTLK_CARD_PCIEG6_D2: \
        {op;} break

/* Gen6 on PCIe */
#define IF_CARD_G6(op)      IF_CARD_PCIEG6(op)

/* Gen7 on PCIe */
#define __CASE_PCIEG7 \
    case MTLK_CARD_PCIEG7:

/* Gen7 on PCIe */
#define IF_CARD_PCIEG7(op) \
    __CASE_PCIEG7 \
        {op;} break

#define IF_CARD_G7(op)      IF_CARD_PCIEG7(op)

/* Either Gen6 or Gen7 */
#define IF_CARD_G6_OR_G7(op) \
    __CASE_PCIEG6 \
    __CASE_PCIEG7 \
        {op;} break

#define IF_CARD_PCI_PCIE(op) \
    __CASE_PCIEG6 \
    __CASE_PCIEG7 \
        {op;} break

#endif /* __HW_MTLK_CARD_SELECTOR_H__ */
