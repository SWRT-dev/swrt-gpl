/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

/*
 * $Id$
 *
 * 
 *
 * Core abilities manager
 *
 * Originally written by Andrii Tseglytskyi
 *
 */

#ifndef _MTLK_AB_MANAGER_H_
#define _MTLK_AB_MANAGER_H_

/** 
*\file mtlk_ab_manager.h 

*\brief Manager of Core "abilities", which are exported to DF

*/

#include "mtlkhash.h"

/* Type for ability ID */
typedef uint32 mtlk_ability_id_t;

/* Pseudo-ability that means "no abilities required" */
#define MTLK_ABILITY_NONE           (0xFFFFFFFFL)
/* Start value for core abilities IDs                */
#define MTLK_CORE_ABILITIES_START  (0x1000000L)

typedef struct _mtlk_abmgr_t mtlk_abmgr_t;

/*! Create ability manager object
    \return  pointer the the new object or NULL
*/
mtlk_abmgr_t* __MTLK_IFUNC
mtlk_abmgr_create(void);

/*! Delete ability manager object
    \param   abmgr          Ability manager object.
*/
void __MTLK_IFUNC
mtlk_abmgr_delete(mtlk_abmgr_t *abmgr);

/*! Register ability

    \param   abmgr         Ability manager object.
    \param   ab_id         Identifier of ability.
    \return  MTLK_ERR... code
*/
int __MTLK_IFUNC
mtlk_abmgr_register_ability(mtlk_abmgr_t *abmgr,
                                 mtlk_ability_id_t ab_id);

/*! Unregister ability
    \param   abmgr          Ability manager object.
    \param   ab_id          Identifier of ability.
*/
void __MTLK_IFUNC
mtlk_abmgr_unregister_ability(mtlk_abmgr_t *abmgr,
                                   mtlk_ability_id_t ab_id);

/*! Whether ability is enabled

    \param   ab_man         Ability manager object.
    \param   ab_id          Identifier of ability.

    \return  TRUE/FALSE
*/
BOOL __MTLK_IFUNC
_mtlk_abmgr_is_ability_enabled(mtlk_abmgr_t *abmgr,
                                    mtlk_ability_id_t ab_id);
#ifdef UNUSED_CODE
/*! Whether ability is registered

    \param   ab_man         Ability manager object.
    \param   ab_id          Identifier of ability.

    \return  TRUE/FALSE
*/
BOOL __MTLK_IFUNC
_mtlk_abmgr_is_ability_registered(mtlk_abmgr_t *abmgr,
                                       mtlk_ability_id_t ab_id);
#endif /* UNUSED_CODE */
/*! Enable ability

    \param   ab_man         Ability manager object.
    \param   ab_id          Identifier of ability.
*/
void __MTLK_IFUNC
mtlk_abmgr_enable_ability(mtlk_abmgr_t*     abmgr,
                               mtlk_ability_id_t  ab_id);

/*! Disable ability

    \param   ab_man         Ability manager object.
    \param   ab_id          Identifier of ability.
*/
void __MTLK_IFUNC
mtlk_abmgr_disable_ability(mtlk_abmgr_t*     abmgr,
                                mtlk_ability_id_t  ab_id);


/*! Register a set of abilities

    \param   abmgr         Ability manager object.
    \param   ab_id_list    Pointer to array of ability IDs
    \param   ab_id_num     Number of entries in the array
    \return  MTLK_ERR... code

    \warning This function is not atomic in sense of the whole set registration,
             i.e. abilities are registered one by one and there are states when only
             part of abilities from set are registered, this does not mean, however,
             the function is not transactional. Upon function return it is guaranteed that
             either all or none abilities from the the set are registered.
*/
int __MTLK_IFUNC
mtlk_abmgr_register_ability_set(mtlk_abmgr_t *abmgr,
                                     const mtlk_ability_id_t* ab_id_list,
                                     uint32 ab_id_num);

/*! Unregister a set of abilities

    \param   abmgr         Ability manager object.
    \param   ab_id_list    Pointer to array of ability IDs
    \param   ab_id_num     Number of entries in the array

    \warning This function is not atomic in sense of the whole set unregistration,
             i.e. abilities are registered one by one and there are states when only
             part of abilities from set are unregistered
*/
void __MTLK_IFUNC
mtlk_abmgr_unregister_ability_set(mtlk_abmgr_t *abmgr,
                                       const mtlk_ability_id_t* ab_id_list,
                                       uint32 ab_id_num);

/*! Enable a set of abilities

    \param   abmgr         Ability manager object.
    \param   ab_id_list    Pointer to array of ability IDs
    \param   ab_id_num     Number of entries in the array

    \warning This function is not atomic in sense of the whole set processing,
             i.e. abilities are enabled one by one and there are states when only
             part of abilities from set are enabled
*/
void __MTLK_IFUNC
mtlk_abmgr_enable_ability_set(mtlk_abmgr_t *abmgr,
                                   const mtlk_ability_id_t* ab_id_list,
                                   uint32 ab_id_num);

/*! Disable a set of abilities

    \param   abmgr         Ability manager object.
    \param   ab_id_list    Pointer to array of ability IDs
    \param   ab_id_num     Number of entries in the array

    \warning This function is not atomic in sense of the whole set processing,
             i.e. abilities are disabled one by one and there are states when only
             part of abilities from set are disabled
*/
void __MTLK_IFUNC
mtlk_abmgr_disable_ability_set(mtlk_abmgr_t *abmgr,
                                    const mtlk_ability_id_t* ab_id_list,
                                    uint32 ab_id_num);

#endif /* _MTLK_AB_MANAGER_H_ */
