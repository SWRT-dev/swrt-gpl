/*
 * This file is part of ltrace.
 * Copyright (C) 2012, 2013 Petr Machata, Red Hat Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

#ifndef DICT_H
#define DICT_H

#include <stddef.h>
#include <stdint.h>
#include <assert.h>
#include "vect.h"

struct dict {
	/* The invariant is that KEYS, VALUES and STATUS are of the
	 * same size.  */
	struct vect keys;
	struct vect values;
	struct vect status;
	size_t size;

	size_t (*hash1)(const void *);
	int (*eq)(const void *, const void *);
	size_t (*hash2)(size_t);
};

/* Initialize a dictionary DICT.  The dictionary will hold keys of the
 * size KEY_SIZE and values of the size VALUE_SIZE.  HASH1 and HASH2
 * are, respectively, primary and secondary hashing functions.  The
 * latter may be NULL, in which case a default internal hash is used.
 * EQ is a callback for comparing two keys.  */
void dict_init(struct dict *dict,
	       size_t key_size, size_t value_size,
	       size_t (*hash1)(const void *),
	       int (*eq)(const void *, const void *),
	       size_t (*hash2)(size_t));

/* Wrapper around dict_init.  Initializes a dictionary DITCP which
 * will hold keys of type KEY_TYPE and values of type VALUE_TYPE.
 * Other arguments as above.  */
#define DICT_INIT(DICTP, KEY_TYPE, VALUE_TYPE, HASH1, EQ, HASH2)	\
	({								\
		/* Check that callbacks are typed properly.  */		\
		size_t (*_hash1_callback)(const KEY_TYPE *) = HASH1;	\
		int (*_eq_callback)(const KEY_TYPE *, const KEY_TYPE *) = EQ; \
		dict_init(DICTP, sizeof(KEY_TYPE), sizeof(VALUE_TYPE),	\
			  (size_t (*)(const void *))_hash1_callback,	\
			  (int (*)(const void *, const void *))_eq_callback, \
			  HASH2);					\
	})

/* Clone SOURCE to TARGET.  For cloning slots, CLONE_KEY and
 * CLONE_VALUE are called.  These callbacks return 0 on success or a
 * negative value on failure.  If any of the callbacks is NULL, the
 * default action is simple memmove.  Returns 0 on success.  If the
 * cloning fails for any reason, already-cloned keys and values are
 * destroyed again by DTOR_KEY and DTOR_VALUE callbacks (if non-NULL),
 * and the function returns a negative value.  DATA is passed to all
 * callbacks verbatim.  */
int dict_clone(struct dict *target, const struct dict *source,
	       int (*clone_key)(void *tgt, const void *src, void *data),
	       void (*dtor_key)(void *tgt, void *data),
	       int (*clone_value)(void *tgt, const void *src, void *data),
	       void (*dtor_value)(void *tgt, void *data),
	       void *data);

/* Clone SRC_DICTP, which holds KEY_TYPE-VALUE_TYPE pairs, into
 * TGT_DICTP.  Other arguments and return codes as above.  */
#define DICT_CLONE(TGT_DICTP, SRC_DICTP, KEY_TYPE, VALUE_TYPE,		\
		   CLONE_KEY, DTOR_KEY, CLONE_VALUE, DTOR_VALUE, DATA)	\
	/* xxx GCC-ism necessary to get in the safety latches.  */	\
	({								\
		const struct dict *_source_d = (SRC_DICTP);		\
		assert(_source_d->keys.elt_size == sizeof(KEY_TYPE));	\
		assert(_source_d->values.elt_size == sizeof(VALUE_TYPE)); \
		/* Check that callbacks are typed properly.  */		\
		void (*_key_dtor_cb)(KEY_TYPE *, void *) = DTOR_KEY;	\
		int (*_key_clone_cb)(KEY_TYPE *, const KEY_TYPE *,	\
				     void *) = CLONE_KEY;		\
		void (*_value_dtor_cb)(VALUE_TYPE *, void *) = DTOR_VALUE; \
		int (*_value_clone_cb)(VALUE_TYPE *, const VALUE_TYPE *, \
				       void *) = CLONE_VALUE;		\
		dict_clone((TGT_DICTP), _source_d,			\
			   (int (*)(void *, const void *,		\
				    void *))_key_clone_cb,		\
			   (void (*)(void *, void *))_key_dtor_cb,	\
			   (int (*)(void *, const void *,		\
				    void *))_value_clone_cb,		\
			   (void (*)(void *, void *))_value_dtor_cb,	\
			   (DATA));					\
	})

/* Return number of key-value pairs stored in DICT.  */
size_t dict_size(const struct dict *dict);

/* Emptiness predicate.  */
int dict_empty(const struct dict *dict);

/* Insert into DICT a pair of KEY and VALUE.  Returns 0 if insertion
 * was successful, a negative value on error, or a positive value if
 * this key is already present in the table.  */
int dict_insert(struct dict *dict, void *key, void *value);

/* Insert into DICT a pair of KEY and VALUE.  See dict_insert for
 * details.  In addition, make a check whether DICTP holds elements of
 * the right size.  */
#define DICT_INSERT(DICTP, KEYP, VALUEP)				\
	(assert((DICTP)->keys.elt_size == sizeof(*(KEYP))),		\
	 assert((DICTP)->values.elt_size == sizeof(*(VALUEP))),		\
	 dict_insert((DICTP), (KEYP), (VALUEP)))

/* Find in DICT a value corresponding to KEY and return a pointer to
 * it.  Returns NULL if the key was not found.  */
void *dict_find(struct dict *dict, const void *key);

/* Look into DICTP for a key *KEYP.  Return a boolean indicating
 * whether the key was found.  */
#define DICT_HAS_KEY(DICTP, KEYP)				\
	(assert((DICTP)->keys.elt_size == sizeof(*(KEYP))),	\
	 dict_find((DICTP), (KEYP)) != NULL)

/* Find in DICTP a value of type VALUE_TYPE corresponding to KEYP and
 * return a pointer (VALUE_TYPE *) to it.  Returns NULL if the key was
 * not found.  */
#define DICT_FIND_REF(DICTP, KEYP, VALUE_TYPE)			\
	(assert((DICTP)->keys.elt_size == sizeof(*(KEYP))),	\
	 (VALUE_TYPE *)dict_find((DICTP), (KEYP)))

/* Find in DICTP a value of type VALUE_TYPE corresponding to KEYP and
 * copy it to the memory pointed-to by VAR.  Returns 0 on success, or
 * a negative value if the key was not found.  */
#define DICT_FIND_VAL(DICTP, KEYP, VAR)					\
	({								\
		assert((DICTP)->keys.elt_size == sizeof(*(KEYP)));	\
		assert((DICTP)->values.elt_size == sizeof((VAR)));	\
		void *_ptr = dict_find((DICTP), (KEYP));		\
		if (_ptr != NULL)					\
			memcpy((VAR), _ptr, (DICTP)->values.elt_size);	\
		_ptr != NULL ? 0 : -1;					\
	})

/* Erase from DICT the entry corresponding to KEY.  Returns a negative
 * value if the key was not found, or 0 on success.  DTOR_KEY and
 * DTOR_VALUE, if non-NULL, are called to destroy the erased
 * value.  */
int dict_erase(struct dict *dict, const void *key,
	       void (*dtor_key)(void *tgt, void *data),
	       void (*dtor_value)(void *tgt, void *data),
	       void *data);

/* Erase from DICTP a value of type VALUE_TYPE corresponding to
 * KEYP.  */
#define DICT_ERASE(DICTP, KEYP, VALUE_TYPE, DTOR_KEY, DTOR_VALUE, DATA) \
	({								\
		struct dict *_d = (DICTP);				\
		assert(_d->keys.elt_size == sizeof(*KEYP));		\
		assert(_d->values.elt_size == sizeof(VALUE_TYPE));	\
		/* Check that callbacks are typed properly.  */		\
		void (*_value_dtor_cb)(VALUE_TYPE *, void *) = DTOR_VALUE; \
		dict_erase(_d, (KEYP), (DTOR_KEY),			\
			   (void (*)(void *, void *))_value_dtor_cb,	\
			   (DATA));					\
	})

/* Destroy DICT.  If KEY_DTOR is non-NULL, then it's called on each
 * key stored in DICT.  Similarly for VALUE_DTOR.  DATA is passed to
 * DTOR's verbatim.  The memory pointed-to by DICT is not freed.  */
void dict_destroy(struct dict *dict,
		  void (*dtor_key)(void *tgt, void *data),
		  void (*dtor_value)(void *tgt, void *data),
		  void *data);

/* Destroy DICTP, which holds keys of type KEY_TYPE and values of type
 * VALUE_TYPE, using DTOR.  */
#define DICT_DESTROY(DICTP, KEY_TYPE, VALUE_TYPE, DTOR_KEY, DTOR_VALUE, DATA) \
	do {								\
		struct dict *_d = (DICTP);				\
		assert(_d->keys.elt_size == sizeof(KEY_TYPE));		\
		assert(_d->values.elt_size == sizeof(VALUE_TYPE));	\
		/* Check that callbacks are typed properly.  */		\
		void (*_key_dtor_cb)(KEY_TYPE *, void *) = DTOR_KEY;	\
		void (*_value_dtor_cb)(VALUE_TYPE *, void *) = DTOR_VALUE; \
		dict_destroy(_d, (void (*)(void *, void *))_key_dtor_cb, \
			     (void (*)(void *, void *))_value_dtor_cb,	\
			     (DATA));					\
	} while (0)

/* Iterate through DICT.  See callback.h for notes on iteration
 * interfaces.  Callback arguments are key, value, DATA.  Note that
 * the iteration over DICT is more expensive than in other containers:
 * while CB is only called for items present in the table, and is
 * therefore O(number of elements), the iterator needs to go through
 * all the table, which is proportional to O(size of table).
 * START_AFTER and the returned iterator are key where the iteration
 * stopped.  */
void *dict_each(struct dict *dict, void *start_after,
		enum callback_status (*cb)(void *, void *, void *), void *data);

#define DICT_EACH(DICTP, KEY_TYPE, VALUE_TYPE, START_AFTER, CB, DATA)	\
	/* xxx GCC-ism necessary to get in the safety latches.  */	\
	({								\
		assert((DICTP)->keys.elt_size == sizeof(KEY_TYPE));	\
		assert((DICTP)->values.elt_size == sizeof(VALUE_TYPE));	\
		/* Check that CB is typed properly.  */			\
		enum callback_status (*_cb)(KEY_TYPE *, VALUE_TYPE *,	\
					    void *) = CB;		\
		KEY_TYPE *_start_after = (START_AFTER);			\
		(KEY_TYPE *)dict_each((DICTP), _start_after,		\
				      (enum callback_status		\
				       (*)(void *, void *, void *))_cb,	\
				      (DATA));				\
	})

/* A callback for hashing integers.  */
size_t dict_hash_int(const int *key);

/* An equality predicate callback for integers.  */
int dict_eq_int(const int *key1, const int *key2);

/* A callback for hashing uint64_t.  */
size_t dict_hash_uint64(const uint64_t *key);

/* An equality predicate callback for uint64_t.  */
int dict_eq_uint64(const uint64_t *key1, const uint64_t *key2);

/* A callback for hashing NULL-terminated strings.  */
size_t dict_hash_string(const char **key);

/* An equality predicate callback for strings.  */
int dict_eq_string(const char **key1, const char **key2);

/* A dtor which calls 'free' on keys in a table.  */
void dict_dtor_string(const char **key, void *data);

/* A cloner that calls 'strdup' on keys in a table.  */
int dict_clone_string(const char **tgt, const char **src, void *data);

#endif /* DICT_H */
