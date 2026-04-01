/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#ifndef __MTLK_LIST_H__
#define __MTLK_LIST_H__

#define SAFE_PLACE_TO_INCLUDE_LIST_OSDEP_DECLS
#include "mtlklist_osdep_decls.h"

/*
  \brief   Calculate the address of the base of the structure given its type, and an
           address of a field within the structure.

  \param   Addr Address of a field within the structure
           Type Type of the structure
           Field Address of a field within the structure
 */
#define MTLK_LIST_GET_CONTAINING_RECORD(Addr, Type, Field) _MTLK_LIST_GET_CONTAINING_RECORD(Addr, Type, Field)


/* WARNING!
 *
 * Functions to operate with locked lists MUST NOT accept list entries as a parameters,
 * these functions may accept list head only.
 */


/*****************************************************************************/
/* Singly-linked lists (stacks)                                              */
/*****************************************************************************/

/*! 
  \fn      static __INLINE void mtlk_slist_init(mtlk_slist_t* slist)
  \brief   Function initializes list object

  \param   slist List object
 */
static __INLINE void mtlk_slist_init(mtlk_slist_t *slist);

/*! 
  \fn      static __INLINE void mtlk_slist_cleanup(mtlk_slist_t* slist)
  \brief   Function cleanups list object, also asserts in case list is not empty.

  \param   slist List object
 */
static __INLINE void mtlk_slist_cleanup(mtlk_slist_t *slist);

/*! 
  \fn      static __INLINE void mtlk_slist_push(mtlk_slist_t* slist, mtlk_slist_entry_t* entry)
  \brief   Function adds entry to a list

  \param   slist List object
           entry List entry
 */
static __INLINE void mtlk_slist_push(mtlk_slist_t       *slist,
                                     mtlk_slist_entry_t *entry);
/*! 
  \fn      static __INLINE mtlk_slist_entry_t* mtlk_slist_pop(mtlk_slist_t* slist)
  \brief   Function removes last entry from a list

  \param   slist List object
  
  \return  Removed entry or NULL if list is empty
 */
static __INLINE mtlk_slist_entry_t *mtlk_slist_pop(mtlk_slist_t *slist);

/*! 
  \fn      static __INLINE mtlk_slist_entry_t* mtlk_slist_remove_next(mtlk_slist_t* slist, mtlk_slist_entry_t* entry)
  \brief   Function removes the next element after given entry

  \param   slist List object
           entry Given entry

  \return  Removed entry or NULL if list is empty
 */
static __INLINE mtlk_slist_entry_t *mtlk_slist_remove_next(mtlk_slist_t       *slist, 
                                                           mtlk_slist_entry_t *entry);

/*!
  \fn      static __INLINE mtlk_slist_entry_t* mtlk_slist_remove(mtlk_slist_t* slist, mtlk_slist_entry_t* entry)
  \brief   Function removes current element

  \param   slist List object
           entry Given entry

  \return  Removed entry or NULL if list is empty
 */
static __INLINE mtlk_slist_entry_t *
mtlk_slist_remove (mtlk_slist_t       *slist,
                   mtlk_slist_entry_t *entry);

/*! 
  \fn      static __INLINE uint32 mtlk_slist_size(mtlk_slist_t* slist)
  \brief   Function returns count of elements which are in list

  \param   slist List object

  \return  Count of elements which are in list
 */
static __INLINE uint32 mtlk_slist_size(mtlk_slist_t *slist);

// Functions for singly list traversal

/*! 
\fn      static __INLINE mtlk_slist_entry_t* mtlk_slist_head(mtlk_slist_t* slist)
\brief   Function returns the head element of a list (virtual entry that points to the first entry)

\param   slist List object

\return  Head of a list
*/
static __INLINE mtlk_slist_entry_t *mtlk_slist_head(mtlk_slist_t *slist);

/*! 
  \fn      static __INLINE mtlk_slist_entry_t* mtlk_slist_begin(mtlk_slist_t* slist)
  \brief   Function returns the first element of a list

  \param   slist List object

  \return  The first element of a list
 */
static __INLINE mtlk_slist_entry_t *mtlk_slist_begin(mtlk_slist_t *slist);

/*! 
  \fn      static __INLINE mtlk_slist_entry_t* mtlk_slist_next(mtlk_slist_entry_t* entry)
  \brief   Function returns the next element after given entry

  \param   entry Given entry

  \return  The next entry after given
 */
static __INLINE mtlk_slist_entry_t *mtlk_slist_next(mtlk_slist_entry_t *entry);

/*! 
  \fn      mtlk_slist_insert_next()
  \brief   Insert given entry after specified entry

  \param   slist List to operate on
  \param   place Position after which insert entry
  \param   entry Entry to insert
 */
static __INLINE void mtlk_slist_insert_next (mtlk_slist_t *slist,
                          mtlk_slist_entry_t *place, mtlk_slist_entry_t *entry);

/*****************************************************************************/
/* Interlocked singly-linked lists (stacks)                                  */
/*****************************************************************************/

/*! 
  \fn      static __INLINE void mtlk_lslist_init(mtlk_lslist_t* lslist)
  \brief   Function initializes list object

  \param   lslist List object
 */
static __INLINE void mtlk_lslist_init(mtlk_lslist_t *lslist);

/*! 
  \fn      static __INLINE void mtlk_lslist_cleanup(mtlk_lslist_t* lslist)
  \brief   Function cleanups list object, also asserts in case list is not empty.

  \param   lslist List object
 */
static __INLINE void mtlk_lslist_cleanup(mtlk_lslist_t *lslist);

/*! 
  \fn      static __INLINE void mtlk_lslist_push(mtlk_lslist_t* lslist, mtlk_lslist_entry_t* entry)
  \brief   Function adds entry to a list

  \param   lslist List object
           entry List entry
 */
static __INLINE void mtlk_lslist_push(mtlk_lslist_t       *lslist,
                                      mtlk_lslist_entry_t *entry);

/*! 
  \fn      static __INLINE mtlk_lslist_entry_t* mtlk_lslist_pop(mtlk_lslist_t* lslist)
  \brief   Function removes last entry from a list

  \param   lslist List object           
  
  \return  Removed entry or NULL if list is empty
 */
static __INLINE mtlk_lslist_entry_t *mtlk_lslist_pop(mtlk_lslist_t *lslist);

/*! 
  \fn      static __INLINE uint32 mtlk_lslist_size(mtlk_lslist_t* lslist)
  \brief   Function returns count of elements which are in list

  \param   lslist List object

  \return  Count of elements which are in list
 */
static __INLINE uint32 mtlk_lslist_size(mtlk_lslist_t *lslist);

/*****************************************************************************/
/* Doubly-linked lists (looped queues)                                       */
/*****************************************************************************/

/*! 
  \fn      static __INLINE void mtlk_dlist_init(mtlk_dlist_t* dlist)
  \brief   Fucntion initializes list object

  \param   dlist List object
 */
static __INLINE void mtlk_dlist_init(mtlk_dlist_t *dlist);

/*! 
  \fn      static __INLINE void mtlk_dlist_cleanup(mtlk_dlist_t* dlist)
  \brief   Function cleanups list object, also asserts in case list is not empty.

  \param   dlist List object
 */
static __INLINE void mtlk_dlist_cleanup(mtlk_dlist_t *dlist);

/*! 
  \fn      static __INLINE void mtlk_dlist_push_front(mtlk_dlist_t* dlist, mtlk_dlist_entry_t* entry)
  \brief   Function adds entry to the beginning of a list

  \param   dlist List object
           entry List entry
 */
static __INLINE void mtlk_dlist_push_front(mtlk_dlist_t       *dlist,
                                           mtlk_dlist_entry_t *entry);

/*! 
  \fn      static __INLINE mtlk_dlist_entry_t* mtlk_dlist_pop_front(mtlk_dlist_t* dlist)
  \brief   Function removes entry from the beginning of a list

  \param   dlist List object           
  
  \return  Removed entry or NULL if list is empty
 */
static __INLINE mtlk_dlist_entry_t *mtlk_dlist_pop_front(mtlk_dlist_t *dlist);

/*! 
  \fn      static __INLINE void mtlk_dlist_push_back(mtlk_dlist_t* dlist, mtlk_dlist_entry_t* entry)
  \brief   Function adds entry to the end of a list

  \param   dlist List object
           entry List entry
 */
static __INLINE void mtlk_dlist_push_back(mtlk_dlist_t       *dlist,
                                          mtlk_dlist_entry_t *entry);

/*! 
  \fn      static __INLINE mtlk_dlist_entry_t* mtlk_dlist_pop_back(mtlk_dlist_t* dlist)
  \brief   Function removes entry from the end of a list

  \param   dlist List object           
  
  \return  Removed entry or NULL if list is empty
 */
static __INLINE mtlk_dlist_entry_t *mtlk_dlist_pop_back(mtlk_dlist_t *dlist);

/*! 
\fn      static __INLINE void mtlk_dlist_insert_before(mtlk_dlist_t* dlist,mtlk_dlist_entry_t* pcur_entry,mtlk_dlist_entry_t* entry)
\brief   Function insert new entry before the current entry

\param   dlist List object
         cur_entry the entry to be insert before
         entry List entry to be insert
*/
static __INLINE void mtlk_dlist_insert_before(mtlk_dlist_t       *dlist,
                                              mtlk_dlist_entry_t *cur_entry,
                                              mtlk_dlist_entry_t *entry);

/*! 
  \fn      mtlk_dlist_remove(mtlk_dlist_t* dlist, mtlk_dlist_entry_t* entry)
  \brief   Function removes entry from the list

  \param   dlist List object
           entry List entry to be removed

  \return  Prev entry after removed
 */
static __INLINE mtlk_dlist_entry_t *mtlk_dlist_remove(mtlk_dlist_t       *dlist,
                                                      mtlk_dlist_entry_t *entry);

/*! 
\fn      static __INLINE mtlk_dlist_entry_t* mtlk_dlist_head(mtlk_dlist_t* dlist)
\brief   Function returns the head element of a list (virtual entry that points to the first entry)

\param   dlist List object

\return  The first element of a list
*/
static __INLINE mtlk_dlist_entry_t *mtlk_dlist_head(mtlk_dlist_t *dlist);

/*! 
  \fn      static __INLINE int8 mtlk_dlist_is_empty(mtlk_dlist_t* dlist)
  \brief   Function checks if list is empty

  \param   dlist List object           
  
  \return  1 if list is empty, 0 - otherwise
 */
static __INLINE int8 mtlk_dlist_is_empty(mtlk_dlist_t *dlist);

/*! 
  \fn      static __INLINE uint32 mtlk_dlist_size(mtlk_dlist_t* dlist)
  \brief   Function returns count of elements which are in list

  \param   dlist List object

  \return  Count of elements which are in list
 */
static __INLINE uint32 mtlk_dlist_size(mtlk_dlist_t *dlist);

/*! 
  \fn      static __INLINE mtlk_dlist_entry_t* mtlk_dlist_remove(mtlk_dlist_t* dlist, mtlk_dlist_entry_t* entry)
  \brief   Function removes given entry from the list

  \param   dlist List object
           entry Given entry

  \return  Removed entry or NULL if list is empty
 */
static __INLINE mtlk_dlist_entry_t *mtlk_dlist_remove(mtlk_dlist_t       *dlist, 
                                                      mtlk_dlist_entry_t *entry);

// Functions for doubly list traversal

/*! 
  \fn      static __INLINE mtlk_dlist_entry_t* mtlk_dlist_next(mtlk_dlist_entry_t* dlist)
  \brief   Function returns the next element after given entry

  \param   entry Given entry

  \return  The next entry after given
 */
static __INLINE mtlk_dlist_entry_t *mtlk_dlist_next(mtlk_dlist_entry_t *entry);

/*! 
  \fn      static __INLINE mtlk_dlist_entry_t* mtlk_dlist_prev(mtlk_dlist_entry_t* entry)
  \brief   Function returns the previous element before given entry

  \param   entry Given entry

  \return  The previous entry after given
 */
static __INLINE mtlk_dlist_entry_t *mtlk_dlist_prev(mtlk_dlist_entry_t *entry);

/*! 
  \fn      static __INLINE void mtlk_dlist_shift(mtlk_dlist_t *dlist)
  \brief   Function shifts the list (moves head to next list element)

  \param   dlist List object
 */
static __INLINE void mtlk_dlist_shift(mtlk_dlist_t *dlist);


/*****************************************************************************/
/* Interlocked doubly-linked lists (looped queues)                           */
/*****************************************************************************/

/*! 
  \fn      static __INLINE void mtlk_ldlist_init(mtlk_ldlist_t* ldlist)
  \brief   Function inits list object.

  \param   ldlist List object
 */
static __INLINE void mtlk_ldlist_init(mtlk_ldlist_t *ldlist);

/*! 
  \fn      static __INLINE void mtlk_ldlist_cleanup(mtlk_ldlist_t* ldlist)
  \brief   Function cleanups list object, also asserts in case list is not empty.

  \param   ldlist List object
 */
static __INLINE void mtlk_ldlist_cleanup(mtlk_ldlist_t *ldlist);

/*! 
  \fn      static __INLINE void mtlk_ldlist_push_front(mtlk_ldlist_t* ldlist, mtlk_ldlist_entry_t* entry)
  \brief   Function adds entry to the beginning of a list

  \param   ldlist List object
           entry List entry
 */
static __INLINE void mtlk_ldlist_push_front(mtlk_ldlist_t       *ldlist,
                                            mtlk_ldlist_entry_t *entry);

/*! 
  \fn      static __INLINE mtlk_ldlist_entry_t* mtlk_ldlist_pop_front(mtlk_ldlist_t* ldlist)
  \brief   Function removes entry from the beginning of a list

  \param   ldlist List object           
  
  \return  Removed entry or NULL if list is empty
 */
static __INLINE mtlk_ldlist_entry_t *mtlk_ldlist_pop_front(mtlk_ldlist_t *ldlist);

/*! 
  \fn      static __INLINE void mtlk_ldlist_push_back(mtlk_ldlist_t* ldlist, mtlk_ldlist_entry_t* entry)
  \brief   Function adds entry to the end of a list

  \param   ldlist List object
           entry List entry
 */
static __INLINE void mtlk_ldlist_push_back(mtlk_ldlist_t       *ldlist,
                                           mtlk_ldlist_entry_t *entry);

/*! 
  \fn      static __INLINE mtlk_ldlist_entry_t* mtlk_ldlist_pop_back(mtlk_ldlist_t* ldlist)
  \brief   Function removes entry from the end of a list

  \param   ldlist List object           
  
  \return  Removed entry or NULL if list is empty
 */
static __INLINE mtlk_ldlist_entry_t *mtlk_ldlist_pop_back(mtlk_ldlist_t *ldlist);

/*! 
  \fn      static __INLINE int8 mtlk_ldlist_is_empty(mtlk_ldlist_t* ldlist)
  \brief   Function checks if list is empty

  \param   ldlist List object           
  
  \return  1 if list is empty, 0 - otherwise
 */
static __INLINE int8 mtlk_ldlist_is_empty(mtlk_ldlist_t *ldlist);

/*! 
  \fn      static __INLINE uint32 mtlk_ldlist_size(mtlk_ldlist_t* ldlist)
  \brief   Function returns count of elements which are in list

  \param   ldlist List object

  \return  Count of elements which are in list
 */
static __INLINE uint32 mtlk_ldlist_size(mtlk_ldlist_t *ldlist);

/*! 
  \fn      static __INLINE mtlk_ldlist_entry_t *mtlk_ldlist_head (mtlk_ldlist_t *ldlist)
  \brief   Function returns head entry of the list

  \param   ldlist List object

  \return  Head entry of the list
 */
static __INLINE mtlk_ldlist_entry_t *mtlk_ldlist_head (mtlk_ldlist_t *ldlist);

/*! 
  \fn      static __INLINE mtlk_ldlist_entry_t * mtlk_ldlist_next (mtlk_ldlist_t *ldlist, mtlk_ldlist_entry_t *lentry)
  \brief   Function returns next of the list

  \param   ldlist List object

  \return  Next entry of the list
 */

static __INLINE mtlk_ldlist_entry_t * mtlk_ldlist_next (mtlk_ldlist_t *ldlist, mtlk_ldlist_entry_t *lentry);

/*! 
  \fn      static __INLINE mtlk_ldlist_entry_t * mtlk_ldlist_prev (mtlk_ldlist_t *ldlist, mtlk_ldlist_entry_t *lentry)
  \brief   Function returns previous of the list

  \param   ldlist List object

  \return  Prev entry of the list
 */

static __INLINE mtlk_ldlist_entry_t * mtlk_ldlist_next (mtlk_ldlist_t *ldlist, mtlk_ldlist_entry_t *lentry);

/***************************************************************
 * Auxilliary list functions
 *
 * WARNING: mtlk_slist_remove()/mtlk_slist_remove_next() 
 *          CAN NOT be used under mtlk_slist_foreach() 
 *          since tmey break the loop.
 ***************************************************************/
#define mtlk_slist_foreach(list, entry, prev_entry)                     \
  for (prev_entry = mtlk_slist_head(list);                              \
       (entry = mtlk_slist_next(prev_entry)) != NULL;                   \
       prev_entry = entry)

#define mtlk_dlist_foreach(list, entry, head)                          \
  for (head = mtlk_dlist_head(list), entry = mtlk_dlist_next(head);    \
       entry != head;                                                  \
       entry = mtlk_dlist_next(entry))

#define SAFE_PLACE_TO_INCLUDE_LIST_OSDEP_DEFS
#include "mtlklist_osdep_defs.h"

#ifdef CPTCFG_IWLWAV_ENABLE_OBJPOOL
#include "mtlklist_dbg.h"
#endif //CPTCFG_IWLWAV_ENABLE_OBJPOOL

#endif // __MTLK_LIST_H__
