/*
 * This file is part of ltrace.
 * Copyright (C) 2011,2012,2013 Petr Machata, Red Hat Inc.
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

#ifndef VECT_H
#define VECT_H

#include <stddef.h>
#include <assert.h>

#include "callback.h"

/* Vector is an array that can grow as needed to accommodate the data
 * that it needs to hold.  ELT_SIZE is also used as an elementary
 * sanity check, because the array itself is not typed.  */

struct vect
{
	void *data;
	size_t size;		/* In elements.  */
	size_t allocated;	/* In elements.  */
	size_t elt_size;	/* In bytes.  */
};

/* Initialize VEC, which will hold elements of size ELT_SIZE.  */
void vect_init(struct vect *vec, size_t elt_size);

/* Initialize VECP, which will hold elements of type ELT_TYPE.  */
#define VECT_INIT(VECP, ELT_TYPE)		\
	(vect_init(VECP, sizeof(ELT_TYPE)))

/* Initialize TARGET by copying over contents of vector SOURCE.  If
 * CLONE is non-NULL, it's evoked on each element, and should clone
 * SRC into TGT.  It should return 0 on success or negative value on
 * failure.  DATA is passed to CLONE verbatim.  This function returns
 * 0 on success or negative value on failure.  In case of failure, if
 * DTOR is non-NULL, it is invoked on all hitherto created elements
 * with the same DATA.  If one of CLONE, DTOR is non-NULL, then both
 * have to be.  */
int vect_clone(struct vect *target, const struct vect *source,
	       int (*clone)(void *tgt, const void *src, void *data),
	       void (*dtor)(void *elt, void *data),
	       void *data);

/* Destroy VEC, which holds elements of type ELT_TYPE, using DTOR.  */
#define VECT_CLONE(TGT_VEC, SRC_VEC, ELT_TYPE, CLONE, DTOR, DATA)	\
	/* xxx GCC-ism necessary to get in the safety latches.  */	\
	({								\
		const struct vect *_source_vec = (SRC_VEC);		\
		assert(_source_vec->elt_size == sizeof(ELT_TYPE));	\
		/* Check that callbacks are typed properly.  */		\
		void (*_dtor_callback)(ELT_TYPE *, void *) = DTOR;	\
		int (*_clone_callback)(ELT_TYPE *, const ELT_TYPE *,	\
				       void *) = CLONE;			\
		vect_clone((TGT_VEC), _source_vec,			\
			   (int (*)(void *, const void *,		\
				    void *))_clone_callback,		\
			   (void (*)(void *, void *))_dtor_callback,	\
			   DATA);					\
	 })

/* Return number of elements in VEC.  */
size_t vect_size(const struct vect *vec);

/* Emptiness predicate.  */
int vect_empty(const struct vect *vec);

/* Accessor.  Fetch ELT_NUM-th argument of type ELT_TYPE from the
 * vector referenced by VECP.  */
#define VECT_ELEMENT(VECP, ELT_TYPE, ELT_NUM)		\
	(assert((VECP)->elt_size == sizeof(ELT_TYPE)),	\
	 assert((ELT_NUM) < (VECP)->size),		\
	 ((ELT_TYPE *)(VECP)->data) + (ELT_NUM))

#define VECT_BACK(VECP, ELT_TYPE)		\
	VECT_ELEMENT(VECP, ELT_TYPE, (VECP)->size - 1)

/* Copy element referenced by ELTP to the end of VEC.  The object
 * referenced by ELTP is now owned by VECT.  Returns 0 if the
 * operation was successful, or negative value on error.  */
int vect_pushback(struct vect *vec, void *eltp);

/* Drop last element of VECP.  This is like calling
 * vect_erase(VEC, vect_size(VEC)-1, vect_size(VEC), DTOR, DATA);  */
void vect_popback(struct vect *vec,
		  void (*dtor)(void *emt, void *data), void *data);

#define VECT_POPBACK(VECP, ELT_TYPE, DTOR, DATA)			\
	do								\
		VECT_ERASE((VECP), ELT_TYPE,				\
			   vect_size(VECP) - 1, vect_size(VECP),	\
			   DTOR, DATA);					\
	while (0)

/* Drop elements START (inclusive) to END (non-inclusive) of VECP.  If
 * DTOR is non-NULL, it is called on each of the removed elements.
 * DATA is passed verbatim to DTOR.  */
void vect_erase(struct vect *vec, size_t start, size_t end,
		void (*dtor)(void *emt, void *data), void *data);

#define VECT_ERASE(VECP, ELT_TYPE, START, END, DTOR, DATA)		\
	do {								\
		assert((VECP)->elt_size == sizeof(ELT_TYPE));		\
		/* Check that DTOR is typed properly.  */		\
		void (*_dtor_callback)(ELT_TYPE *, void *) = DTOR;	\
		vect_erase((VECP), (START), (END),			\
			   (void (*)(void *, void *))_dtor_callback, DATA); \
	} while (0)

/* Copy element referenced by ELTP to the end of VEC.  See
 * vect_pushback for details.  In addition, make a check whether VECP
 * holds elements of the right size.  */
#define VECT_PUSHBACK(VECP, ELTP)			\
	(assert((VECP)->elt_size == sizeof(*(ELTP))),	\
	 vect_pushback((VECP), (ELTP)))

/* Make sure that VEC can hold at least COUNT elements.  Return 0 on
 * success, negative value on failure.  */
int vect_reserve(struct vect *vec, size_t count);

/* Make sure that VEC can accommodate COUNT additional elements.  */
int vect_reserve_additional(struct vect *vec, size_t count);

/* Destroy VEC.  If DTOR is non-NULL, then it's called on each element
 * of the vector.  DATA is passed to DTOR verbatim.  The memory
 * pointed-to by VEC is not freed.  */
void vect_destroy(struct vect *vec,
		  void (*dtor)(void *emt, void *data), void *data);

/* Destroy VEC, which holds elements of type ELT_TYPE, using DTOR.  */
#define VECT_DESTROY(VECP, ELT_TYPE, DTOR, DATA)			\
	do {								\
		assert((VECP)->elt_size == sizeof(ELT_TYPE));		\
		/* Check that DTOR is typed properly.  */		\
		void (*_dtor_callback)(ELT_TYPE *, void *) = DTOR;	\
		vect_destroy((VECP), (void (*)(void *, void *))_dtor_callback, \
			     DATA);					\
	} while (0)

/* Iterate through vector VEC.  See callback.h for notes on iteration
 * interfaces.  */
void *vect_each(struct vect *vec, void *start_after,
		enum callback_status (*cb)(void *, void *), void *data);

#define VECT_EACH(VECP, ELT_TYPE, START_AFTER, CB, DATA)		\
	/* xxx GCC-ism necessary to get in the safety latches.  */	\
	({								\
		assert((VECP)->elt_size == sizeof(ELT_TYPE));		\
		/* Check that CB is typed properly.  */			\
		enum callback_status (*_cb)(ELT_TYPE *, void *) = CB;	\
		ELT_TYPE *_start_after = (START_AFTER);			\
		(ELT_TYPE *)vect_each((VECP), _start_after,		\
				      (enum callback_status		\
				       (*)(void *, void *))_cb,		\
				      DATA);				\
	})

/* Iterate through vector VEC.  See callback.h for notes on iteration
 * interfaces.  */
const void *vect_each_cst(const struct vect *vec, const void *start_after,
			  enum callback_status (*cb)(const void *, void *),
			  void *data);

#define VECT_EACH_CST(VECP, ELT_TYPE, START_AFTER, CB, DATA)		\
	/* xxx GCC-ism necessary to get in the safety latches.  */	\
	({								\
		assert((VECP)->elt_size == sizeof(ELT_TYPE));		\
		/* Check that CB is typed properly.  */			\
		enum callback_status (*_cb)(const ELT_TYPE *, void *) = CB; \
		const ELT_TYPE *start_after = (START_AFTER);		\
		(const ELT_TYPE *)vect_each_cst((VECP), start_after,	\
						(enum callback_status	\
						 (*)(const void *,	\
						     void *))_cb,	\
						DATA);			\
	})

/* Call qsort on elements of VECT, with COMPAR as a comparison
 * function.  */
void vect_qsort(struct vect *vec, int (*compar)(const void *, const void *));

#define VECT_QSORT(VECP, ELT_TYPE, COMPAR)				\
	do {								\
		assert((VECP)->elt_size == sizeof(ELT_TYPE));		\
		/* Check that CB is typed properly.  */			\
		int (*_compar)(const ELT_TYPE *, const ELT_TYPE *) = COMPAR; \
		vect_qsort((VECP),					\
			   (int (*)(const void *, const void *))_compar); \
	} while (0)


/* A dtor which calls 'free' on elements of a vector.  */
void vect_dtor_string(char **key, void *data);

#endif /* VECT_H */
