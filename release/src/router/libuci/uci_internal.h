/*
 * libuci - Library for the Unified Configuration Interface
 * Copyright (C) 2008 Felix Fietkau <nbd@openwrt.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 2.1
 * as published by the Free Software Foundation
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 */

#ifndef __UCI_INTERNAL_H
#define __UCI_INTERNAL_H

#define __private __attribute__((visibility("hidden")))
#define __public
#ifdef UCI_PLUGIN_SUPPORT
#define __plugin extern
#else
#define __plugin __private
#endif

struct uci_parse_context
{
	/* error context */
	const char *reason;
	int line;
	int byte;

	/* private: */
	struct uci_package *package;
	struct uci_section *section;
	bool merge;
	FILE *file;
	const char *name;
	char *buf;
	int bufsz;
};

extern const char *uci_confdir;
extern const char *uci_savedir;

__plugin void *uci_malloc(struct uci_context *ctx, size_t size);
__plugin void *uci_realloc(struct uci_context *ctx, void *ptr, size_t size);
__plugin char *uci_strdup(struct uci_context *ctx, const char *str);
__plugin bool uci_validate_str(const char *str, bool name);
__plugin void uci_add_delta(struct uci_context *ctx, struct uci_list *list, int cmd, const char *section, const char *option, const char *value);
__plugin void uci_free_delta(struct uci_delta *h);
__plugin struct uci_package *uci_alloc_package(struct uci_context *ctx, const char *name);

__private FILE *uci_open_stream(struct uci_context *ctx, const char *filename, int pos, bool write, bool create);
__private void uci_close_stream(FILE *stream);
__private void uci_getln(struct uci_context *ctx, int offset);

__private void uci_parse_error(struct uci_context *ctx, char *pos, char *reason);
__private void uci_alloc_parse_context(struct uci_context *ctx);

__private void uci_cleanup(struct uci_context *ctx);
__private struct uci_element *uci_lookup_list(struct uci_list *list, const char *name);
__private void uci_fixup_section(struct uci_context *ctx, struct uci_section *s);
__private void uci_free_package(struct uci_package **package);
__private struct uci_element *uci_alloc_generic(struct uci_context *ctx, int type, const char *name, int size);
__private void uci_free_element(struct uci_element *e);
__private struct uci_element *uci_expand_ptr(struct uci_context *ctx, struct uci_ptr *ptr, bool complete);

__private int uci_load_delta(struct uci_context *ctx, struct uci_package *p, bool flush);

static inline bool uci_validate_package(const char *str)
{
	return uci_validate_str(str, false);
}

static inline bool uci_validate_type(const char *str)
{
	return uci_validate_str(str, false);
}

static inline bool uci_validate_name(const char *str)
{
	return uci_validate_str(str, true);
}

/* initialize a list head/item */
static inline void uci_list_init(struct uci_list *ptr)
{
	ptr->prev = ptr;
	ptr->next = ptr;
}

/* inserts a new list entry after a given entry */
static inline void uci_list_insert(struct uci_list *list, struct uci_list *ptr)
{
	list->next->prev = ptr;
	ptr->prev = list;
	ptr->next = list->next;
	list->next = ptr;
}

/* inserts a new list entry at the tail of the list */
static inline void uci_list_add(struct uci_list *head, struct uci_list *ptr)
{
	/* NB: head->prev points at the tail */
	uci_list_insert(head->prev, ptr);
}

static inline void uci_list_del(struct uci_list *ptr)
{
	struct uci_list *next, *prev;

	next = ptr->next;
	prev = ptr->prev;

	prev->next = next;
	next->prev = prev;

	uci_list_init(ptr);
}


extern struct uci_backend uci_file_backend;

#ifdef UCI_PLUGIN_SUPPORT
/**
 * uci_add_backend: add an extra backend
 * @ctx: uci context
 * @name: name of the backend
 *
 * The default backend is "file", which uses /etc/config for config storage
 */
__plugin int uci_add_backend(struct uci_context *ctx, struct uci_backend *b);

/**
 * uci_add_backend: add an extra backend
 * @ctx: uci context
 * @name: name of the backend
 *
 * The default backend is "file", which uses /etc/config for config storage
 */
__plugin int uci_del_backend(struct uci_context *ctx, struct uci_backend *b);
#endif

#define UCI_BACKEND(_var, _name, ...)	\
struct uci_backend _var = {		\
	.e.list = {			\
		.next = &_var.e.list,	\
		.prev = &_var.e.list,	\
	},				\
	.e.name = _name,		\
	.e.type = UCI_TYPE_BACKEND,	\
	.ptr = &_var,			\
	__VA_ARGS__			\
}


/*
 * functions for debug and error handling, for internal use only
 */

#ifdef UCI_DEBUG
#define DPRINTF(...) fprintf(stderr, __VA_ARGS__)
#else
#define DPRINTF(...)
#endif

/* 
 * throw an uci exception and store the error number
 * in the context.
 */
#define UCI_THROW(ctx, err) do { 	\
	DPRINTF("Exception: %s in %s, %s:%d\n", #err, __func__, __FILE__, __LINE__); \
	longjmp(ctx->trap, err); 	\
} while (0)

/*
 * store the return address for handling exceptions
 * needs to be called in every externally visible library function
 *
 * NB: this does not handle recursion at all. Calling externally visible
 * functions from other uci functions is only allowed at the end of the
 * calling function, or by wrapping the function call in UCI_TRAP_SAVE
 * and UCI_TRAP_RESTORE.
 */
#define UCI_HANDLE_ERR(ctx) do {	\
	DPRINTF("ENTER: %s\n", __func__); \
	int __val = 0;			\
	ctx->err = 0;			\
	if (!ctx)			\
		return UCI_ERR_INVAL;	\
	if (!ctx->internal && !ctx->nested) \
		__val = setjmp(ctx->trap); \
	ctx->internal = false;		\
	ctx->nested = false;		\
	if (__val) {			\
		DPRINTF("LEAVE: %s, ret=%d\n", __func__, __val); \
		ctx->err = __val;	\
		return __val;		\
	}				\
} while (0)

/*
 * In a block enclosed by UCI_TRAP_SAVE and UCI_TRAP_RESTORE, all exceptions
 * are intercepted and redirected to the label specified in 'handler'
 * after UCI_TRAP_RESTORE, or when reaching the 'handler' label, the old
 * exception handler is restored
 */
#define UCI_TRAP_SAVE(ctx, handler) do {   \
	jmp_buf	__old_trap;		\
	int __val;			\
	memcpy(__old_trap, ctx->trap, sizeof(ctx->trap)); \
	__val = setjmp(ctx->trap);	\
	if (__val) {			\
		ctx->err = __val;	\
		memcpy(ctx->trap, __old_trap, sizeof(ctx->trap)); \
		goto handler;		\
	}
#define UCI_TRAP_RESTORE(ctx)		\
	memcpy(ctx->trap, __old_trap, sizeof(ctx->trap)); \
} while(0)

/**
 * UCI_INTERNAL: Do an internal call of a public API function
 * 
 * Sets Exception handling to passthrough mode.
 * Allows API functions to change behavior compared to public use
 */
#define UCI_INTERNAL(func, ctx, ...) do { \
	ctx->internal = true;		\
	func(ctx, __VA_ARGS__);		\
} while (0)

/**
 * UCI_NESTED: Do an normal nested call of a public API function
 * 
 * Sets Exception handling to passthrough mode.
 * Allows API functions to change behavior compared to public use
 */
#define UCI_NESTED(func, ctx, ...) do { \
	ctx->nested = true;		\
	func(ctx, __VA_ARGS__);		\
} while (0)


/*
 * check the specified condition.
 * throw an invalid argument exception if it's false
 */
#define UCI_ASSERT(ctx, expr) do {	\
	if (!(expr)) {			\
		DPRINTF("[%s:%d] Assertion failed\n", __FILE__, __LINE__); \
		UCI_THROW(ctx, UCI_ERR_INVAL);	\
	}				\
} while (0)

#endif
