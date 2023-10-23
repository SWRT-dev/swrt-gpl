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
 * $Id: $
 *
 * 
 *  
 * Linked lists implementation for Linux driver:
 *   1. Singly-linked lists
 *   2. Thread-safe singly-linked lists
 */

#if !defined (SAFE_PLACE_TO_INCLUDE_LIST_OSDEP_DEFS)
#error "You shouldn't include this file directly!"
#endif /* SAFE_PLACE_TO_INCLUDE_LIST_OSDEP_... */

#undef SAFE_PLACE_TO_INCLUDE_LIST_OSDEP_DEFS

#define _MTLK_LIST_GET_CONTAINING_RECORD(ptr, type, member) container_of(ptr, type, member)

#define LOG_LOCAL_GID   GID_MTLK_LIST
#define LOG_LOCAL_FID   0

/*
 * 1. Singly-linked lists (stacks)
 */

static __INLINE void
mtlk_slist_init (mtlk_slist_t *slist)
{
  ASSERT(slist != NULL);
  slist->head.next = NULL;
  slist->depth = 0;
}

static __INLINE void
mtlk_slist_cleanup (mtlk_slist_t *slist)
{
  ASSERT(slist != NULL);
  ASSERT(slist->depth == 0);
  slist->head.next = NULL;
}

static __INLINE void
mtlk_slist_push (mtlk_slist_t       *slist,
                 mtlk_slist_entry_t *entry)
{
  ASSERT(slist != NULL);
  ASSERT(entry != NULL);
  entry->next = slist->head.next;
  slist->head.next = entry;
  ++slist->depth;
}

static __INLINE mtlk_slist_entry_t *
mtlk_slist_pop (mtlk_slist_t *slist)
{
  mtlk_slist_entry_t *entry;
  ASSERT(slist != NULL);
  entry = slist->head.next;
  if (entry != NULL) {
    slist->head.next = entry->next;
    --slist->depth;
  }
  return entry;
}

static __INLINE mtlk_slist_entry_t *
mtlk_slist_remove_next (mtlk_slist_t       *slist, 
                        mtlk_slist_entry_t *entry)
{
  mtlk_slist_entry_t *entry_next;
  ASSERT(slist != NULL);
  ASSERT(entry != NULL);
  entry_next = entry->next;
  if (entry_next != NULL) {
    entry->next = entry_next->next;
    --slist->depth;
  }
  return entry_next;
}

static __INLINE mtlk_slist_entry_t *
mtlk_slist_remove (mtlk_slist_t       *slist,
                   mtlk_slist_entry_t *entry)
{
  mtlk_slist_entry_t *cur;
  mtlk_slist_entry_t *prev = mtlk_slist_begin(slist);

  if (!entry)
    return NULL;
  /* check if the very first element is to be deleted */
  if (prev == entry) {
    mtlk_slist_pop(slist);
  } else {
    while ((cur = mtlk_slist_next(prev)) != entry)
      prev = cur;
    if (cur)
      mtlk_slist_remove_next(slist, prev);
  }
  return entry;
}

static __INLINE uint32
mtlk_slist_size (mtlk_slist_t *slist)
{
  ASSERT(slist != NULL);
  return slist->depth;
}

static __INLINE mtlk_slist_entry_t *
mtlk_slist_head (mtlk_slist_t *slist)
{
  ASSERT(slist != NULL);
  return &slist->head;
}

static __INLINE mtlk_slist_entry_t *
mtlk_slist_begin (mtlk_slist_t *slist)
{
  ASSERT(slist != NULL);
  return slist->head.next;
}

static __INLINE mtlk_slist_entry_t *
mtlk_slist_next (mtlk_slist_entry_t *entry)
{
  ASSERT(entry != NULL);
  return entry->next;
}

static __INLINE void
mtlk_slist_insert_next (mtlk_slist_t *slist, mtlk_slist_entry_t *place, mtlk_slist_entry_t *entry)
{
  ASSERT(slist != NULL);
  ASSERT(place != NULL);
  ASSERT(entry != NULL);
  entry->next = place->next;
  place->next = entry;
  slist->depth++;
}

/*
 * 2. Thread-safe singly-linked lists (stacks)
 */

static __INLINE void
mtlk_lslist_init (mtlk_lslist_t *lslist)
{
  ASSERT(lslist != NULL);
  mtlk_slist_init(&lslist->list);
  spin_lock_init(&lslist->lock);
}

static __INLINE void
mtlk_lslist_cleanup (mtlk_lslist_t *lslist)
{
  ASSERT(lslist != NULL);
  mtlk_slist_cleanup(&lslist->list);
}

static __INLINE void
mtlk_lslist_push (mtlk_lslist_t       *lslist,
                  mtlk_lslist_entry_t *entry)
{
  ASSERT(lslist != NULL);
  spin_lock_bh(&lslist->lock);
  mtlk_slist_push(&lslist->list, entry);
  spin_unlock_bh(&lslist->lock);
}

static __INLINE mtlk_lslist_entry_t*
mtlk_lslist_pop (mtlk_lslist_t *lslist)
{
  mtlk_lslist_entry_t *entry;
  ASSERT(lslist != NULL);
  spin_lock_bh(&lslist->lock);
  entry = mtlk_slist_pop(&lslist->list);
  spin_unlock_bh(&lslist->lock);
  return entry;
}

static __INLINE uint32
mtlk_lslist_size (mtlk_lslist_t *lslist)
{
  return mtlk_slist_size(&lslist->list);
}

/*
 * 3. Doubly-linked lists (looped queues)
 */

static __INLINE void
mtlk_dlist_init(mtlk_dlist_t *dlist)
{
  ASSERT(dlist != NULL);
  INIT_LIST_HEAD(&dlist->head);
  dlist->depth = 0;
}

static __INLINE void
mtlk_dlist_cleanup (mtlk_dlist_t *dlist)
{
  ASSERT(dlist != NULL);
  ASSERT(dlist->depth == 0);
  INIT_LIST_HEAD(&dlist->head);
  dlist->depth = 0;
}

static __INLINE void
mtlk_dlist_move (mtlk_dlist_t *dest_dlist, mtlk_dlist_t *src_dlist)
{
  ASSERT(src_dlist != NULL);
  ASSERT(dest_dlist != NULL);
  ASSERT(dest_dlist->depth == 0);
  list_replace_init(&src_dlist->head, &dest_dlist->head);
  dest_dlist->depth = src_dlist->depth;
  src_dlist->depth = 0;
}

static __INLINE void
mtlk_dlist_push_front (mtlk_dlist_t       *dlist,
                       mtlk_dlist_entry_t *entry)
{
  ASSERT(dlist != NULL);
  ASSERT(entry != NULL);
  list_add(entry, &dlist->head);
  ++dlist->depth;
}

static __INLINE mtlk_dlist_entry_t *
mtlk_dlist_pop_front (mtlk_dlist_t *dlist)
{
  mtlk_dlist_entry_t *entry;
  ASSERT(dlist != NULL);
  if (list_empty(&dlist->head))
    return NULL;
  entry = dlist->head.next;
  ASSERT(entry != NULL);
  list_del(entry);
  --dlist->depth;
  return entry;
}

static __INLINE void
mtlk_dlist_push_back (mtlk_dlist_t       *dlist,
                      mtlk_dlist_entry_t *entry)
{
  ASSERT(dlist != NULL);
  ASSERT(entry != NULL);
  list_add_tail(entry, &dlist->head);
  ++dlist->depth;
}

/*
 * klocwork sometimes doesn't understand which object got added to the list through
 * mtlk_dlist_push_back, as that function only receives pointer to the
 * mtlk_dlist_entry_t and not the object itself.
 *
 * __mtlk_dlist_push_back_obj is our workaround. It receives pointer to the object
 * as its second argument even though it isn't used. This allows us to create
 * a rule in the klocwork database that promises to track object somewhere else.
 */

static __INLINE void
__mtlk_dlist_push_back_obj(mtlk_dlist_t *list, void *obj, mtlk_dlist_entry_t *entry)
{
  mtlk_dlist_push_back(list, entry);
}

#define mtlk_dlist_push_back_obj(list, obj, entry) \
  __mtlk_dlist_push_back_obj(list, obj, &((obj)->entry))

static __INLINE mtlk_dlist_entry_t *
mtlk_dlist_pop_back (mtlk_dlist_t *dlist)
{
  mtlk_dlist_entry_t *entry;
  ASSERT(dlist != NULL);
  if (list_empty(&dlist->head))
    return NULL;
  entry = dlist->head.prev;
  ASSERT(entry != NULL);
  list_del(entry);
  --dlist->depth;
  return entry;
}

static __INLINE void
mtlk_dlist_insert_before (mtlk_dlist_t       *dlist,
                          mtlk_dlist_entry_t *cur_entry,
                          mtlk_dlist_entry_t *entry)
{
  ASSERT(dlist != NULL);
  ASSERT(entry != NULL);
  ASSERT(cur_entry != NULL);
  list_add(entry, cur_entry->prev);
  ++dlist->depth;
}

static __INLINE mtlk_dlist_entry_t *
mtlk_dlist_remove (mtlk_dlist_t       *dlist, 
                   mtlk_dlist_entry_t *entry)
{
  mtlk_ldlist_entry_t *entry_prev;
  ASSERT(dlist != NULL);
  ASSERT(entry != &dlist->head);
  ASSERT(!list_empty(&dlist->head));
  ASSERT(entry != NULL);
  entry_prev = mtlk_dlist_prev(entry);
  list_del(entry);
  --dlist->depth;
  return entry_prev;
}

static __INLINE mtlk_dlist_entry_t *
mtlk_dlist_head (mtlk_dlist_t *dlist)
{
  ASSERT(dlist != NULL);
  return &dlist->head;
}

static __INLINE int8
mtlk_dlist_is_empty (mtlk_dlist_t *dlist)
{
  ASSERT(dlist != NULL);
  return list_empty(&dlist->head);
}

static __INLINE uint32
mtlk_dlist_size (mtlk_dlist_t *dlist)
{
  ASSERT(dlist != NULL);
  return dlist->depth;
}

static __INLINE mtlk_dlist_entry_t *
mtlk_dlist_next (mtlk_dlist_entry_t *entry)
{
  ASSERT(entry != NULL);
  return entry->next;
}

static __INLINE mtlk_dlist_entry_t *
mtlk_dlist_prev (mtlk_dlist_entry_t *entry)
{
  ASSERT(entry != NULL);
  return entry->prev;
}

static __INLINE void
mtlk_dlist_shift (mtlk_dlist_t *dlist)
{
  ASSERT(dlist != NULL);

  if (dlist->depth > 1) {
    mtlk_dlist_entry_t *entry = dlist->head.next;
    list_move_tail(entry, &dlist->head);
  }
}

/*
 * 4. Interlocked doubly-linked lists (looped queues)
 */

static __INLINE void
mtlk_ldlist_init (mtlk_ldlist_t *ldlist)
{
  mtlk_dlist_init(&ldlist->list);
  spin_lock_init(&ldlist->lock);
}

static __INLINE void
mtlk_ldlist_cleanup (mtlk_ldlist_t *ldlist)
{
  mtlk_dlist_cleanup(&ldlist->list);
}

static __INLINE void
mtlk_ldlist_push_front (mtlk_ldlist_t       *ldlist,
                        mtlk_ldlist_entry_t *entry)
{
  ASSERT(ldlist != NULL);
  spin_lock_bh(&ldlist->lock);
  mtlk_dlist_push_front (&ldlist->list, entry);
  spin_unlock_bh(&ldlist->lock);
}

static __INLINE mtlk_ldlist_entry_t *
mtlk_ldlist_pop_front (mtlk_ldlist_t *ldlist)
{
  mtlk_ldlist_entry_t *entry;
  ASSERT(ldlist != NULL);
  spin_lock_bh(&ldlist->lock);
  entry = mtlk_dlist_pop_front(&ldlist->list);
  spin_unlock_bh(&ldlist->lock);
  return entry;
}

static __INLINE void
mtlk_ldlist_push_back (mtlk_ldlist_t       *ldlist,
                       mtlk_ldlist_entry_t *entry)
{
  ASSERT(ldlist != NULL);
  spin_lock_bh(&ldlist->lock);
  mtlk_dlist_push_back(&ldlist->list, entry);
  spin_unlock_bh(&ldlist->lock);
}

static __INLINE mtlk_ldlist_entry_t *
mtlk_ldlist_pop_back (mtlk_ldlist_t *ldlist)
{
  mtlk_ldlist_entry_t *entry;
  ASSERT(ldlist != NULL);
  spin_lock_bh(&ldlist->lock);
  entry = mtlk_dlist_pop_back(&ldlist->list);
  spin_unlock_bh(&ldlist->lock);
  return entry;
}

static __INLINE mtlk_ldlist_entry_t *
mtlk_ldlist_remove (mtlk_ldlist_t       *ldlist, 
                    mtlk_ldlist_entry_t *entry)
{
  mtlk_ldlist_entry_t *entry_prev;
  ASSERT(ldlist != NULL);
  ASSERT(entry != NULL);

  spin_lock_bh(&ldlist->lock);
  entry_prev = mtlk_dlist_remove(&ldlist->list, entry);
  spin_unlock_bh(&ldlist->lock);
  return entry_prev;
}

static __INLINE int8
mtlk_ldlist_is_empty (mtlk_ldlist_t *ldlist)
{
  return mtlk_dlist_is_empty(&ldlist->list);
}

static __INLINE uint32
mtlk_ldlist_size (mtlk_ldlist_t *ldlist)
{
  return mtlk_dlist_size(&ldlist->list);
}

static __INLINE mtlk_ldlist_entry_t *
mtlk_ldlist_head (mtlk_ldlist_t *ldlist)
{
  mtlk_ldlist_entry_t *entry;

  ASSERT(ldlist != NULL);

  spin_lock_bh(&ldlist->lock);
  entry = mtlk_dlist_head(&ldlist->list);
  spin_unlock_bh(&ldlist->lock);

  return entry;
}

static __INLINE mtlk_ldlist_entry_t *
mtlk_ldlist_next (mtlk_ldlist_t *ldlist, mtlk_ldlist_entry_t *lentry)
{
  mtlk_ldlist_entry_t *entry;

  ASSERT(lentry != NULL);

  spin_lock_bh(&ldlist->lock);
  entry = mtlk_dlist_next(lentry);
  spin_unlock_bh(&ldlist->lock);

  return entry;
}

static __INLINE mtlk_ldlist_entry_t *
mtlk_ldlist_prev (mtlk_ldlist_t *ldlist, mtlk_ldlist_entry_t *lentry)
{
  mtlk_ldlist_entry_t *entry;

  ASSERT(lentry != NULL);

  spin_lock_bh(&ldlist->lock);
  entry = mtlk_dlist_prev(lentry);
  spin_unlock_bh(&ldlist->lock);

  return entry;
}

#undef LOG_LOCAL_GID
#undef LOG_LOCAL_FID
