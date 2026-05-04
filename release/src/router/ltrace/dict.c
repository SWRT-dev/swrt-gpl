/*
 * This file is part of ltrace.
 * Copyright (C) 2012, 2013, 2014, 2015 Petr Machata, Red Hat Inc.
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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "dict.h"

struct status_bits {
	unsigned char taken : 1;
	unsigned char erased : 1;
};

static struct status_bits *
bitp(struct dict *dict, size_t n)
{
	return VECT_ELEMENT(&dict->status, struct status_bits, n);
}

void
dict_init(struct dict *dict,
	  size_t key_size, size_t value_size,
	  size_t (*hash1)(const void *),
	  int (*eq)(const void *, const void *),
	  size_t (*hash2)(size_t))
{
	assert(hash1 != NULL);
	assert(eq != NULL);

	vect_init(&dict->keys, key_size);
	vect_init(&dict->values, value_size);
	VECT_INIT(&dict->status, struct status_bits);
	dict->size = 0;

	dict->hash1 = hash1;
	dict->hash2 = hash2;
	dict->eq = eq;
}

struct clone_data {
	struct dict *target;
	int (*clone_key)(void *tgt, const void *src, void *data);
	int (*clone_value)(void *tgt, const void *src, void *data);
	void (*dtor_key)(void *tgt, void *data);
	void (*dtor_value)(void *tgt, void *data);
	void *data;
};

static enum callback_status
clone_cb(void *key, void *value, void *data)
{
	struct clone_data *clone_data = data;

	char nkey[clone_data->target->keys.elt_size];
	if (clone_data->clone_key == NULL)
		memmove(nkey, key, sizeof(nkey));
	else if (clone_data->clone_key(&nkey, key, clone_data->data) < 0)
		return CBS_STOP;

	char nvalue[clone_data->target->values.elt_size];
	if (clone_data->clone_value == NULL) {
		memmove(nvalue, value, sizeof(nvalue));
	} else if (clone_data->clone_value(&nvalue, value,
					 clone_data->data) < 0) {
	fail:
		if (clone_data->clone_key != NULL)
			clone_data->dtor_key(&nkey, clone_data->data);
		return CBS_STOP;
	}

	if (dict_insert(clone_data->target, nkey, nvalue) < 0) {
		if (clone_data->clone_value != NULL)
			clone_data->dtor_value(&nvalue, clone_data->data);
		goto fail;
	}

	return CBS_CONT;
}

int
dict_clone(struct dict *target, const struct dict *source,
	   int (*clone_key)(void *tgt, const void *src, void *data),
	   void (*dtor_key)(void *tgt, void *data),
	   int (*clone_value)(void *tgt, const void *src, void *data),
	   void (*dtor_value)(void *tgt, void *data),
	   void *data)
{
	assert((clone_key != NULL) == (dtor_key != NULL));
	assert((clone_value != NULL) == (dtor_value != NULL));

	dict_init(target, source->keys.elt_size, source->values.elt_size,
		  source->hash1, source->eq, source->hash2);
	struct clone_data clone_data = {
		target, clone_key, clone_value, dtor_key, dtor_value, data
	};
	if (dict_each((struct dict *)source, NULL,
		      clone_cb, &clone_data) != NULL) {
		dict_destroy(target, dtor_key, dtor_value, data);
		return -1;
	}
	return 0;
}

size_t
dict_size(const struct dict *dict)
{
	return dict->size;
}

int
dict_empty(const struct dict *dict)
{
	return dict->size == 0;
}

struct destroy_data {
	void (*dtor_key)(void *tgt, void *data);
	void (*dtor_value)(void *tgt, void *data);
	void *data;
};

static enum callback_status
destroy_cb(void *key, void *value, void *data)
{
	struct destroy_data *destroy_data = data;
	if (destroy_data->dtor_key)
		destroy_data->dtor_key(key, destroy_data->data);
	if (destroy_data->dtor_value)
		destroy_data->dtor_value(value, destroy_data->data);
	return CBS_CONT;
}

void
dict_destroy(struct dict *dict,
	     void (*dtor_key)(void *tgt, void *data),
	     void (*dtor_value)(void *tgt, void *data),
	     void *data)
{
	/* Some slots are unused (the corresponding keys and values
	 * are uninitialized), so we can't call dtors for them.
	 * Iterate DICT instead.  */
	if (dtor_key != NULL || dtor_value != NULL) {
		struct destroy_data destroy_data = {
			dtor_key, dtor_value, data
		};
		dict_each(dict, NULL, destroy_cb, &destroy_data);
	}

	vect_destroy(&dict->keys, NULL, NULL);
	vect_destroy(&dict->values, NULL, NULL);
	vect_destroy(&dict->status, NULL, NULL);
}

static size_t
default_secondary_hash(size_t pos)
{
	return pos % 97 + 1;
}

static size_t
small_secondary_hash(size_t pos)
{
	return 1;
}

static inline size_t
n(struct dict *dict)
{
	return vect_size(&dict->keys);
}

static inline size_t (*
hash2(struct dict *dict))(size_t)
{
	if (dict->hash2 != NULL)
		return dict->hash2;
	else if (n(dict) < 100)
		return small_secondary_hash;
	else
		return default_secondary_hash;
}

static void *
getkey(struct dict *dict, size_t pos)
{
	return ((unsigned char *)dict->keys.data)
		+ dict->keys.elt_size * pos;
}

static void *
getvalue(struct dict *dict, size_t pos)
{
	return ((unsigned char *)dict->values.data)
		+ dict->values.elt_size * pos;
}

static size_t
find_slot(struct dict *dict, const void *key,
	  int *foundp, int *should_rehash, size_t *pi)
{
	assert(n(dict) > 0);
	size_t pos = dict->hash1(key) % n(dict);
	size_t pos0 = -1;
	size_t d = hash2(dict)(pos);
	size_t i = 0;
	*foundp = 0;

	/* We skip over any taken or erased slots.  But we remember
	 * the first erased that we find, and if we don't find the key
	 * later, we return that position.  */
	for (; bitp(dict, pos)->taken || bitp(dict, pos)->erased;
	     pos = (pos + d) % n(dict)) {
		if (pos0 == (size_t)-1 && bitp(dict, pos)->erased)
			pos0 = pos;

		/* If there is a loop, but we've seen an erased
		 * element, take that one.  Otherwise give up.  */
		if (++i > n(dict)) {
			if (pos0 != (size_t)-1)
				break;
			return (size_t)-1;
		}

		if (bitp(dict, pos)->taken
		    && dict->eq(getkey(dict, pos), key)) {
			*foundp = 1;
			break;
		}
	}

	if (!*foundp && pos0 != (size_t)-1)
		pos = pos0;

	/* If the hash table degraded into a linked list, request a
	 * rehash.  */
	if (should_rehash != NULL)
		*should_rehash = i > 10 && i > n(dict) / 10;

	if (pi != NULL)
		*pi = i;
	return pos;
}

static enum callback_status
rehash_move(void *key, void *value, void *data)
{
	if (dict_insert(data, key, value) < 0)
		return CBS_STOP;
	else
		return CBS_CONT;
}

static int
rehash(struct dict *dict, size_t nn)
{
	assert(nn != n(dict));
	int ret = -1;

	struct dict tmp;
	dict_init(&tmp, dict->keys.elt_size, dict->values.elt_size,
		  dict->hash1, dict->eq, dict->hash2);

	/* To honor all invariants (so that we can safely call
	 * dict_destroy), we first make a request to _reserve_ enough
	 * room in all vectors.  This has no observable effect on
	 * contents of vectors.  */
	if (vect_reserve(&tmp.keys, nn) < 0
	    || vect_reserve(&tmp.values, nn) < 0
	    || vect_reserve(&tmp.status, nn) < 0)
		goto done;

	/* Now that we know that there is enough size in vectors, we
	 * simply bump the size.  */
	tmp.keys.size = nn;
	tmp.values.size = nn;
	size_t old_size = tmp.status.size;
	tmp.status.size = nn;
	memset(VECT_ELEMENT(&tmp.status, struct status_bits, old_size),
	       0, (tmp.status.size - old_size) * tmp.status.elt_size);

	/* At this point, TMP is once more an empty dictionary with NN
	 * slots.  Now move stuff from DICT to TMP.  */
	if (dict_each(dict, NULL, rehash_move, &tmp) != NULL)
		goto done;

	/* And now swap contents of DICT and TMP, and we are done.  */
	{
		struct dict tmp2 = *dict;
		*dict = tmp;
		tmp = tmp2;
	}

	ret = 0;

done:
	/* We only want to release the containers, not the actual data
	 * that they hold, so it's fine if we don't pass any dtor.  */
	dict_destroy(&tmp, NULL, NULL, NULL);
	return ret;

}

static const size_t primes[] = {
	13, 31, 61, 127, 251, 509, 1021, 2039, 4093,
	8191, 16381, 32749, 65521, 130981, 0
};

static size_t
larger_size(size_t current)
{
	if (current == 0)
		return primes[0];

	if (current < primes[sizeof(primes)/sizeof(*primes) - 2]) {
		size_t i;
		for (i = 0; primes[i] != 0; ++i)
			if (primes[i] > current)
				return primes[i];
		abort();
	}

	/* We ran out of primes, so invent a new one.  The following
	 * gives primes until about 17M elements (and then some more
	 * later).  */
	return 2 * current + 6585;
}

static size_t
smaller_size(size_t current)
{
	if (current <= primes[0])
		return primes[0];

	if (current <= primes[sizeof(primes)/sizeof(*primes) - 2]) {
		size_t i;
		size_t prev = 0;
		for (i = 0; primes[i] != 0; ++i) {
			if (primes[i] >= current)
				return prev;
			prev = primes[i];
		}
		abort();
	}

	return (current - 6585) / 2;
}

int
dict_insert(struct dict *dict, void *key, void *value)
{
	if (n(dict) == 0 || dict->size > 0.7 * n(dict))
	rehash:
		if (rehash(dict, larger_size(n(dict))) < 0)
			return -1;

	int found;
	int should_rehash;
	size_t slot_n = find_slot(dict, key, &found, &should_rehash, NULL);
	if (slot_n == (size_t)-1)
		return -1;
	if (found)
		return 1;
	assert(!bitp(dict, slot_n)->taken);

	/* If rehash was requested, do that, and retry.  But just live
	 * with it for apparently sparse tables.  No resizing can fix
	 * a rubbish hash.  */
	if (should_rehash && dict->size > 0.3 * n(dict))
		goto rehash;

	memmove(getkey(dict, slot_n), key, dict->keys.elt_size);
	memmove(getvalue(dict, slot_n), value, dict->values.elt_size);

	bitp(dict, slot_n)->taken = 1;
	bitp(dict, slot_n)->erased = 0;
	++dict->size;

	return 0;
}

void *
dict_find(struct dict *dict, const void *key)
{
	if (dict->size == 0)
		return NULL;
	assert(n(dict) > 0);

	int found;
	size_t slot_n = find_slot(dict, key, &found, NULL, NULL);
	if (found)
		return getvalue(dict, slot_n);
	else
		return NULL;
}

int
dict_erase(struct dict *dict, const void *key,
	   void (*dtor_key)(void *tgt, void *data),
	   void (*dtor_value)(void *tgt, void *data),
	   void *data)
{
	int found;
	size_t i;
	size_t slot_n = find_slot(dict, key, &found, NULL, &i);
	if (!found)
		return -1;

	if (dtor_key != NULL)
		dtor_key(getkey(dict, slot_n), data);
	if (dtor_value != NULL)
		dtor_value(getvalue(dict, slot_n), data);

	bitp(dict, slot_n)->taken = 0;
	bitp(dict, slot_n)->erased = 1;
	--dict->size;

	if (dict->size < 0.3 * n(dict)) {
		size_t smaller = smaller_size(n(dict));
		if (smaller != n(dict))
			/* Don't mind if it fails when shrinking.  */
			rehash(dict, smaller);
	}

	return 0;
}

void *
dict_each(struct dict *dict, void *start_after,
	  enum callback_status (*cb)(void *, void *, void *), void *data)
{
	size_t i;
	if (start_after != NULL)
		i = ((start_after - dict->keys.data) / dict->keys.elt_size) + 1;
	else
		i = 0;

	for (; i < dict->keys.size; ++i)
		if (bitp(dict, i)->taken && !bitp(dict, i)->erased) {
			void *key = getkey(dict, i);
			if (cb(key, getvalue(dict, i), data) != CBS_CONT)
				return key;
		}

	return NULL;
}

size_t
dict_hash_int(const int *key)
{
	return (size_t)(*key * 2654435761U);
}

int
dict_eq_int(const int *key1, const int *key2)
{
	return *key1 == *key2;
}

size_t
dict_hash_uint64(const uint64_t *key)
{
	int const a = (int) *key;
	int const b = (int) (*key >> 32);
	return dict_hash_int (&a) ^ dict_hash_int (&b);
}

int
dict_eq_uint64(const uint64_t *key1, const uint64_t *key2)
{
	return *key1 == *key2;
}

size_t
dict_hash_string(const char **key)
{
	size_t h = 5381;
	const char *str = *key;
	while (*str != 0)
		h = h * 33 ^ *str++;
	return h;
}

int
dict_eq_string(const char **key1, const char **key2)
{
	return strcmp(*key1, *key2) == 0;
}

void
dict_dtor_string(const char **key, void *data)
{
	free((char *)*key);
}

int
dict_clone_string(const char **tgt, const char **src, void *data)
{
	*tgt = strdup(*src);
	return *tgt != NULL ? 0 : -1;
}

#ifdef TEST
static enum callback_status
dump(int *key, int *value, void *data)
{
	char *seen = data;
	assert(seen[*key] == 0);
	seen[*key] = 1;
	assert(*value == *key * 2 + 1);
	return CBS_STOP;
}

static size_t
dict_hash_int_silly(const int *key)
{
	return *key % 10;
}

static void
verify(struct dict *di, size_t len, char *seen)
{
	size_t ct = 0;
	int *it;
	for (it = NULL; (it = DICT_EACH(di, int, int, it, dump, seen)) != NULL;)
		ct++;
	assert(ct == len);
	memset(seen, 0, len);
}

static enum callback_status
fill_keys(int *key, int *value, void *data)
{
	int *array = data;
	array[++array[0]] = *key;
	return CBS_CONT;
}

static void
test1(void)
{
	struct dict di;
	DICT_INIT(&di, int, int, dict_hash_int, dict_eq_int, NULL);

	char seen[100000] = {};
	size_t i;
	for (i = 0; i < sizeof(seen); ++i) {
		int key = i;
		int value = 2 * i + 1;
		DICT_INSERT(&di, &key, &value);
		int *valp = DICT_FIND_REF(&di, &key, int);
		assert(valp != NULL);
		assert(*valp == value);
		assert(dict_size(&di) == i + 1);
	}

	verify(&di, sizeof(seen), seen);

	struct dict d2;
	DICT_CLONE(&d2, &di, int, int, NULL, NULL, NULL, NULL, NULL);
	DICT_DESTROY(&di, int, int, NULL, NULL, NULL);
	verify(&d2, sizeof(seen), seen);

	/* Now we try to gradually erase all elements.  We can't erase
	 * inside a DICT_EACH call, so copy first keys to a separate
	 * memory area first.  */
	int keys[d2.size + 1];
	size_t ct = 0;
	keys[0] = 0;
	DICT_EACH(&d2, int, int, NULL, fill_keys, keys);
	for (i = 0; i < (size_t)keys[0]; ++i) {
		assert(DICT_ERASE(&d2, &keys[i + 1], int,
				  NULL, NULL, NULL) == 0);
		++ct;
	}
	assert(ct == sizeof(seen));
	DICT_DESTROY(&d2, int, int, NULL, NULL, NULL);
}

static void
test_erase(void)
{
	int i;

	/* To test erase, we need a relatively bad hash function, so
	 * that there are some overlapping chains in the table.  */
	struct dict d2;
	DICT_INIT(&d2, int, int, dict_hash_int_silly, dict_eq_int, NULL);
	const int limit = 500;
	for (i = 0; i < limit; ++i) {
		int key = 2 * i + 1;
		int value = 2 * key + 1;
		DICT_INSERT(&d2, &key, &value);
	}

	/* Now we try to delete each of the keys, and verify that none
	 * of the chains was broken.  */
	for (i = 0; i < limit; ++i) {
		struct dict copy;
		DICT_CLONE(&copy, &d2, int, int, NULL, NULL, NULL, NULL, NULL);
		int key = 2 * i + 1;
		DICT_ERASE(&copy, &key, int, NULL, NULL, NULL);
		assert(dict_size(&copy) == dict_size(&d2) - 1);

		int j;
		for (j = 0; j < limit; ++j) {
			key = 2 * j + 1;
			int *valp = DICT_FIND_REF(&copy, &key, int);
			if (i != j) {
				assert(valp != NULL);
				assert(*valp == 2 * key + 1);
			} else {
				assert(valp == NULL);
			}
		}

		DICT_DESTROY(&copy, int, int, NULL, NULL, NULL);
	}
	DICT_DESTROY(&d2, int, int, NULL, NULL, NULL);
}

static void
test_erase_find(void)
{
	/* Degenerate case for erase followed by find, considering 2 colliding keys.
	   Erase 1st inserted key, then try finding 2nd.
	 */
	struct dict d2;
	DICT_INIT(&d2, int, int, dict_hash_int_silly, dict_eq_int, NULL);
	int key;
	int value;

	key = 0, value = 0;
	DICT_INSERT(&d2, &key, &value);
	key = 10, value = 10;
	DICT_INSERT(&d2, &key, &value);

	key = 10;
	assert (DICT_FIND_REF(&d2, &key, int) != NULL);

	key = 0;
	DICT_ERASE(&d2, &key, int, NULL, NULL, NULL);

	key = 10;
	assert (DICT_FIND_REF(&d2, &key, int) != NULL);

	DICT_DESTROY(&d2, int, int, NULL, NULL, NULL);
}

int main(int argc, char *argv[])
{
	test1();
	test_erase();
	test_erase_find();
	return 0;
}

#endif
