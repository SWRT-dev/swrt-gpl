/*
 * This file is part of ltrace.
 * Copyright (C) 2011,2012,2013,2014 Petr Machata, Red Hat Inc.
 * Copyright (C) 2010 Joe Damato
 * Copyright (C) 1997,1998,1999,2001,2002,2003,2004,2007,2008,2009 Juan Cespedes
 * Copyright (C) 2006 Paul Gilliam, IBM Corporation
 * Copyright (C) 2006 Ian Wienand
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

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <inttypes.h>

#include "output.h"
#include "demangle.h"
#include "fetch.h"
#include "lens_default.h"
#include "library.h"
#include "memstream.h"
#include "options.h"
#include "param.h"
#include "proc.h"
#include "prototype.h"
#include "summary.h"
#include "type.h"
#include "value.h"
#include "value_dict.h"
#include "filter.h"
#include "debug.h"

#if defined(HAVE_LIBDW)
#include "dwarf_prototypes.h"
#endif

static struct process *current_proc = NULL;
static size_t current_depth = 0;
static int current_column = 0;

static void
output_indent(struct process *proc)
{
	int d = options.indent * (proc->callstack_depth - 1);
	current_column += fprintf(options.output, "%*s", d, "");
}

static void
begin_of_line(struct process *proc, int is_func, int indent)
{
	current_column = 0;
	if (!proc) {
		return;
	}
	if ((options.output != stderr) && (opt_p || options.follow)) {
		current_column += fprintf(options.output, "%u ", proc->pid);
	} else if (options.follow) {
		current_column += fprintf(options.output, "[pid %u] ", proc->pid);
	}
	if (opt_r) {
		struct timeval tv;
		static struct timeval old_tv = { 0, 0 };
		struct timeval diff;

		gettimeofday(&tv, NULL);

		if (old_tv.tv_sec == 0 && old_tv.tv_usec == 0) {
			old_tv.tv_sec = tv.tv_sec;
			old_tv.tv_usec = tv.tv_usec;
		}
		diff.tv_sec = tv.tv_sec - old_tv.tv_sec;
		if (tv.tv_usec >= old_tv.tv_usec) {
			diff.tv_usec = tv.tv_usec - old_tv.tv_usec;
		} else {
			diff.tv_sec--;
			diff.tv_usec = 1000000 + tv.tv_usec - old_tv.tv_usec;
		}
		old_tv.tv_sec = tv.tv_sec;
		old_tv.tv_usec = tv.tv_usec;
		current_column += fprintf(options.output, "%3lu.%06d ",
					  (unsigned long)diff.tv_sec,
					  (int)diff.tv_usec);
	}
	if (opt_t) {
		struct timeval tv;
		gettimeofday(&tv, NULL);
		if (opt_t > 2) {
			current_column += fprintf(options.output, "%lu.%06d ",
						  (unsigned long)tv.tv_sec,
						  (int)tv.tv_usec);
		} else if (opt_t > 1) {
			struct tm *tmp = localtime(&tv.tv_sec);
			current_column +=
			    fprintf(options.output, "%02d:%02d:%02d.%06d ",
				    tmp->tm_hour, tmp->tm_min, tmp->tm_sec,
				    (int)tv.tv_usec);
		} else {
			struct tm *tmp = localtime(&tv.tv_sec);
			current_column += fprintf(options.output, "%02d:%02d:%02d ",
						  tmp->tm_hour, tmp->tm_min,
						  tmp->tm_sec);
		}
	}
	if (opt_i) {
		if (is_func) {
			struct callstack_element *stel
				= &proc->callstack[proc->callstack_depth - 1];
			current_column += fprintf(options.output, "[%p] ",
						  stel->return_addr);
		} else {
			current_column += fprintf(options.output, "[%p] ",
						  proc->instruction_pointer);
		}
	}
	if (options.indent > 0 && indent) {
		output_indent(proc);
	}
}

static struct arg_type_info *
get_unknown_type(void)
{
	static struct arg_type_info *ret = NULL;
	if (ret != NULL)
		return ret;

	static struct arg_type_info info;
	info = *type_get_simple(ARGTYPE_LONG);
	info.lens = &guess_lens;
	ret = &info;
	return ret;
}

/* The default prototype is: long X(long, long, long, long).  */
static struct prototype *
build_default_prototype(void)
{
	static struct prototype *ret = NULL;
	if (ret != NULL)
		return ret;

	static struct prototype proto;
	prototype_init(&proto);

	struct arg_type_info *unknown_type = get_unknown_type();
	assert(unknown_type != NULL);
	proto.return_info = unknown_type;
	proto.own_return_info = 0;

	struct param unknown_param;
	param_init_type(&unknown_param, unknown_type, 0);

	size_t i;
	for (i = 0; i < 4; ++i)
		if (prototype_push_param(&proto, &unknown_param) < 0) {
			report_global_error("build_default_prototype: %s",
					    strerror(errno));
			prototype_destroy(&proto);
			return NULL;
		}

	ret = &proto;
	return ret;
}

static bool
snip_period(char *buf)
{
	char *period = strrchr(buf, '.');
	if (period != NULL && strcmp(period, ".so") != 0) {
		*period = 0;
		return true;
	} else {
		return false;
	}
}

struct lookup_prototype_alias_context
{
	struct library *lib;
	struct prototype *result; // output
};
static enum callback_status
lookup_prototype_alias_cb(const char *name, void *data)
{
	struct lookup_prototype_alias_context *context =
		(struct lookup_prototype_alias_context*)data;

	struct library *lib = context->lib;

	context->result =
		protolib_lookup_prototype(lib->protolib, name,
					  lib->type != LT_LIBTYPE_SYSCALL);
	if (context->result != NULL)
		return CBS_STOP;

	return CBS_CONT;
}

static struct prototype *
library_get_prototype(struct library *lib, const char *name)
{
	if (lib->protolib == NULL) {
		size_t sz = strlen(lib->soname);
		char buf[sz + 1];
		memcpy(buf, lib->soname, sz + 1);

		do {
			if (protolib_cache_maybe_load(&g_protocache, buf, 0,
						      true, &lib->protolib) < 0)
				return NULL;
		} while (lib->protolib == NULL
			 && lib->type == LT_LIBTYPE_DSO
			 && snip_period(buf));

#if defined(HAVE_LIBDW)
		// DWARF data fills in the gaps in the .conf files, so I don't
		// check for lib->protolib==NULL here
		if (lib->dwfl_module != NULL &&
		    (filter_matches_library(options.plt_filter,    lib ) ||
		     filter_matches_library(options.static_filter, lib ) ||
		     filter_matches_library(options.export_filter, lib )))
			import_DWARF_prototypes(lib);
		else
			debug(DEBUG_FUNCTION,
			      "Filter didn't match prototype '%s' in lib '%s'. "
			      "Not importing",
			      name, lib->soname);
#endif

		if (lib->protolib == NULL)
			lib->protolib = protolib_cache_default(&g_protocache,
							       buf, 0);
	}
	if (lib->protolib == NULL)
		return NULL;

	struct prototype *result =
		protolib_lookup_prototype(lib->protolib, name,
					  lib->type != LT_LIBTYPE_SYSCALL);
	if (result != NULL)
		return result;

	// prototype not found. Is it aliased?
	struct lookup_prototype_alias_context context = {.lib = lib,
							 .result = NULL};
	library_exported_names_each_alias(&lib->exported_names, name,
					  NULL, lookup_prototype_alias_cb,
					  &context);

	// if found, the prototype is stored here, otherwise it's NULL
	return context.result;
}

struct find_proto_data {
	const char *name;
	struct prototype *ret;
};

static enum callback_status
find_proto_cb(struct process *proc, struct library *lib, void *d)
{
	struct find_proto_data *data = d;
	data->ret = library_get_prototype(lib, data->name);
	return CBS_STOP_IF(data->ret != NULL);
}

static struct prototype *
lookup_symbol_prototype(struct process *proc, struct library_symbol *libsym)
{
	if (libsym->proto != NULL)
		return libsym->proto;

	struct library *lib = libsym->lib;
	if (lib != NULL) {
		struct find_proto_data data = { libsym->name };
		data.ret = library_get_prototype(lib, libsym->name);
		if (data.ret == NULL
		    && libsym->plt_type == LS_TOPLT_EXEC)
			proc_each_library(proc, NULL, find_proto_cb, &data);
		if (data.ret != NULL)
			return data.ret;
	}

	return build_default_prototype();
}

void
output_line(struct process *proc, const char *fmt, ...)
{
	if (options.summary)
		return;

	if (current_proc != NULL) {
		if (current_proc->callstack[current_depth].return_addr)
			fprintf(options.output, " <unfinished ...>\n");
		else
			fprintf(options.output, " <no return ...>\n");
	}
	current_proc = NULL;
	if (fmt == NULL)
		return;

	begin_of_line(proc, 0, 0);

	va_list args;
	va_start(args, fmt);
	vfprintf(options.output, fmt, args);
	fprintf(options.output, "\n");
	va_end(args);

	current_column = 0;
}

static void
tabto(int col) {
	if (current_column < col) {
		fprintf(options.output, "%*s", col - current_column, "");
	}
}

static int
output_error(FILE *stream)
{
	return fprintf(stream, "?");
}

static int
fetch_simple_param(enum tof type, struct process *proc,
		   struct fetch_context *context,
		   struct value_dict *arguments,
		   struct arg_type_info *info, int own,
		   struct value *valuep)
{
	/* Arrays decay into pointers per C standard.  We check for
	 * this here, because here we also capture arrays that come
	 * from parameter packs.  */
	if (info->type == ARGTYPE_ARRAY) {
		struct arg_type_info *tmp = malloc(sizeof(*tmp));
		if (tmp != NULL) {
			type_init_pointer(tmp, info, own);
			tmp->lens = info->lens;
			info = tmp;
			own = 1;
		}
	}

	struct value value;
	value_init(&value, proc, NULL, info, own);
	if (fetch_arg_next(context, type, proc, info, &value) < 0)
		return -1;

	if (val_dict_push_next(arguments, &value) < 0) {
		value_destroy(&value);
		return -1;
	}

	if (valuep != NULL)
		*valuep = value;

	return 0;
}

static void
fetch_param_stop(struct value_dict *arguments, ssize_t *params_leftp)
{
	if (*params_leftp == -1)
		*params_leftp = val_dict_count(arguments);
}

static int
fetch_param_pack(enum tof type, struct process *proc,
		 struct fetch_context *context,
		 struct value_dict *arguments, struct param *param,
		 ssize_t *params_leftp)
{
	struct param_enum *e = param_pack_init(param, arguments);
	if (e == NULL)
		return -1;

	int ret = 0;
	while (1) {
		int insert_stop = 0;
		struct arg_type_info *info = malloc(sizeof(*info));
		if (info == NULL
		    || param_pack_next(param, e, info, &insert_stop) < 0) {
		fail:
			free(info);
			ret = -1;
			break;
		}

		if (insert_stop)
			fetch_param_stop(arguments, params_leftp);

		if (info->type == ARGTYPE_VOID) {
			type_destroy(info);
			free(info);
			break;
		}

		struct value val;
		if (fetch_simple_param(type, proc, context, arguments,
				       info, 1, &val) < 0)
			goto fail;

		int stop = 0;
		switch (param_pack_stop(param, e, &val)) {
		case PPCB_ERR:
			goto fail;
		case PPCB_STOP:
			stop = 1;
		case PPCB_CONT:
			break;
		}

		if (stop)
			break;
	}

	param_pack_done(param, e);
	return ret;
}

static int
fetch_one_param(enum tof type, struct process *proc,
		struct fetch_context *context,
		struct value_dict *arguments, struct param *param,
		ssize_t *params_leftp)
{
	switch (param->flavor) {
		int rc;
	case PARAM_FLAVOR_TYPE:
		return fetch_simple_param(type, proc, context, arguments,
					  param->u.type.type, 0, NULL);

	case PARAM_FLAVOR_PACK:
		if (fetch_param_pack_start(context,
					   param->u.pack.ppflavor) < 0)
			return -1;
	        rc = fetch_param_pack(type, proc, context, arguments,
				      param, params_leftp);
		fetch_param_pack_end(context);
		return rc;

	case PARAM_FLAVOR_STOP:
		fetch_param_stop(arguments, params_leftp);
		return 0;
	}

	assert(!"Invalid param flavor!");
	abort();
}

struct fetch_one_param_data
{
	struct process *proc;
	struct fetch_context *context;
	struct value_dict *arguments;
	ssize_t *params_leftp;
	enum tof tof;
};

static enum callback_status
fetch_one_param_cb(struct prototype *proto, struct param *param, void *data)
{
	struct fetch_one_param_data *cb_data = data;
	return CBS_STOP_IF(fetch_one_param(cb_data->tof, cb_data->proc,
					   cb_data->context,
					   cb_data->arguments, param,
					   cb_data->params_leftp) < 0);
}

static int
fetch_params(enum tof type, struct process *proc,
	     struct fetch_context *context,
	     struct value_dict *arguments, struct prototype *func,
	     ssize_t *params_leftp)
{
	struct fetch_one_param_data cb_data
		= { proc, context, arguments, params_leftp, type };
	if (prototype_each_param(func, NULL,
				 &fetch_one_param_cb, &cb_data) != NULL)
		return -1;

	/* Implicit stop at the end of parameter list.  */
	fetch_param_stop(arguments, params_leftp);

	return 0;
}

struct format_argument_data
{
	struct value *value;
	struct value_dict *arguments;
};

static int
format_argument_cb(FILE *stream, void *ptr)
{
	struct format_argument_data *data = ptr;
	int o = format_argument(stream, data->value, data->arguments);
	if (o < 0)
		o = output_error(stream);
	return o;
}

static int
output_params(struct value_dict *arguments, size_t start, size_t end,
	      int *need_delimp)
{
	size_t i;
	for (i = start; i < end; ++i) {
		struct value *value = val_dict_get_num(arguments, i);
		if (value == NULL)
			return -1;

		struct format_argument_data data = { value, arguments };
		int o = delim_output(options.output, need_delimp,
				     format_argument_cb, &data);
		if (o < 0)
			return -1;
		current_column += o;
	}
	return 0;
}

void
output_left(enum tof type, struct process *proc,
	    struct library_symbol *libsym)
{
	assert(! options.summary);

	if (current_proc) {
		fprintf(options.output, " <unfinished ...>\n");
		current_column = 0;
	}
	current_proc = proc;
	current_depth = proc->callstack_depth;
	begin_of_line(proc, type == LT_TOF_FUNCTION, 1);
	if (!options.hide_caller && libsym->lib != NULL
	    && libsym->plt_type != LS_TOPLT_NONE)
		/* We don't terribly mind failing this.  */
		account_output(&current_column,
			       fprintf(options.output, "%s->",
				       libsym->lib->soname));

	const char *name = libsym->name;
#ifdef USE_DEMANGLE
	if (options.demangle)
		name = my_demangle(libsym->name);
#endif
	if (account_output(&current_column,
			   fprintf(options.output, "%s", name)) < 0)
		return;

	if (libsym->lib != NULL
	    && libsym->lib->type != LT_LIBTYPE_MAIN
	    && libsym->plt_type == LS_TOPLT_NONE
	    && account_output(&current_column,
			      fprintf(options.output, "@%s",
				      libsym->lib->soname)) < 0)
		/* We do mind failing this though.  */
		return;

	account_output(&current_column, fprintf(options.output, "("));

	struct prototype *func = lookup_symbol_prototype(proc->leader, libsym);
	if (func == NULL) {
	fail:
		account_output(&current_column, fprintf(options.output, "???"));
		return;
	}

	struct fetch_context *context = fetch_arg_init(type, proc,
						       func->return_info);
	if (context == NULL)
		goto fail;

	struct value_dict *arguments = malloc(sizeof(*arguments));
	if (arguments == NULL) {
		fetch_arg_done(context);
		goto fail;
	}
	val_dict_init(arguments);

	ssize_t params_left = -1;
	int need_delim = 0;
	if (fetch_params(type, proc, context, arguments, func, &params_left) < 0
	    || output_params(arguments, 0, params_left, &need_delim) < 0) {
		val_dict_destroy(arguments);
		fetch_arg_done(context);
		arguments = NULL;
		context = NULL;
	}

	struct callstack_element *stel
		= &proc->callstack[proc->callstack_depth - 1];
	stel->fetch_context = context;
	stel->arguments = arguments;
	stel->out.params_left = params_left;
	stel->out.need_delim = need_delim;
}

#if defined(HAVE_LIBDW)
/* Prints information about one frame of a thread.  Called by
   dwfl_getthread_frames in output_right.  Returns 1 when done (max
   number of frames reached).  Returns -1 on error.  Returns 0 on
   success (if there are more frames in the thread, call us again).  */
static int
frame_callback (Dwfl_Frame *state, void *arg)
{
	Dwarf_Addr pc;
	bool isactivation;

	int *frames = (int *) arg;

	if (!dwfl_frame_pc(state, &pc, &isactivation))
		return -1;

	if (!isactivation)
		pc--;

	Dwfl *dwfl = dwfl_thread_dwfl(dwfl_frame_thread(state));
	Dwfl_Module *mod = dwfl_addrmodule(dwfl, pc);
	const char *modname = NULL;
	const char *symname = NULL;
	GElf_Off off = 0;
	if (mod != NULL) {
		GElf_Sym sym;
		modname = dwfl_module_info(mod, NULL, NULL, NULL, NULL,
					   NULL, NULL, NULL);
		symname = dwfl_module_addrinfo(mod, pc, &off, &sym,
					       NULL, NULL, NULL);
	}

	/* This mimics the output produced by libunwind below.  */
	fprintf(options.output, " > %s(%s+0x%" PRIx64 ") [%" PRIx64 "]\n",
		modname, symname, off, pc);

	/* See if we can extract the source line too and print it on
	   the next line if we can find it.  */
	if (mod != NULL) {
		Dwfl_Line *l = dwfl_module_getsrc(mod, pc);
		if (l != NULL) {
			int line, col;
			line = col = -1;
			const char *src = dwfl_lineinfo(l, NULL, &line, &col,
							NULL, NULL);
			if (src != NULL) {
				fprintf(options.output, "\t%s", src);
				if (line > 0) {
					fprintf(options.output, ":%d", line);
					if (col > 0)
			                        fprintf(options.output,
							":%d", col);
				}
				fprintf(options.output, "\n");
			}

		}
	}

	/* Max number of frames to print reached? */
	if ((*frames)-- == 0)
		return 1;

	return 0;
}
#endif /* defined(HAVE_LIBDW) */

void
output_right(enum tof type, struct process *proc, struct library_symbol *libsym,
	     struct timedelta *spent)
{
	assert(! options.summary);

	struct prototype *func = lookup_symbol_prototype(proc, libsym);
	if (func == NULL)
		return;

	if (current_proc != NULL
		    && (current_proc != proc
			|| current_depth != proc->callstack_depth)) {
		fprintf(options.output, " <unfinished ...>\n");
		current_proc = NULL;
	}
	if (current_proc != proc) {
		begin_of_line(proc, type == LT_TOF_FUNCTIONR, 1);
#ifdef USE_DEMANGLE
		current_column +=
		    fprintf(options.output, "<... %s resumed> ",
			    options.demangle ? my_demangle(libsym->name)
			    : libsym->name);
#else
		current_column +=
		    fprintf(options.output, "<... %s resumed> ", libsym->name);
#endif
	}

	struct callstack_element *stel
		= &proc->callstack[proc->callstack_depth - 1];

	struct fetch_context *context = stel->fetch_context;

	/* Fetch & enter into dictionary the retval first, so that
	 * other values can use it in expressions.  */
	struct value retval;
	bool own_retval = false;
	if (context != NULL) {
		value_init(&retval, proc, NULL, func->return_info, 0);
		own_retval = true;
		if (fetch_retval(context, type, proc, func->return_info,
				 &retval) < 0)
			value_set_type(&retval, NULL, 0);
		else if (stel->arguments != NULL
			 && val_dict_push_named(stel->arguments, &retval,
						"retval", 0) == 0)
			own_retval = false;
	}

	if (stel->arguments != NULL)
		output_params(stel->arguments, stel->out.params_left,
			      val_dict_count(stel->arguments),
			      &stel->out.need_delim);

	current_column += fprintf(options.output, ") ");
	tabto(options.align - 1);
	fprintf(options.output, "= ");

	if (context != NULL && retval.type != NULL) {
		struct format_argument_data data = { &retval, stel->arguments };
		format_argument_cb(options.output, &data);
	}

	if (own_retval)
		value_destroy(&retval);

	if (opt_T) {
		assert(spent != NULL);
		fprintf(options.output, " <%lu.%06d>",
			(unsigned long) spent->tm.tv_sec,
			(int) spent->tm.tv_usec);
	}

	fprintf(options.output, "\n");

#if defined(HAVE_LIBUNWIND)
	if (options.bt_depth > 0
	    && proc->unwind_priv != NULL
	    && proc->unwind_as != NULL) {
		unw_cursor_t cursor;
		arch_addr_t ip, function_offset;
		struct library *lib = NULL;
		int unwind_depth = options.bt_depth;
		char fn_name[100];
		const char *lib_name;
		size_t distance;

		/* Verify that we can safely cast arch_addr_t* to
		 * unw_word_t*.  */
		(void)sizeof(char[1 - 2*(sizeof(unw_word_t)
					!= sizeof(arch_addr_t))]);
		unw_init_remote(&cursor, proc->unwind_as, proc->unwind_priv);
		while (unwind_depth) {

			int rc = unw_get_reg(&cursor, UNW_REG_IP,
					     (unw_word_t *) &ip);
			if (rc < 0) {
				fprintf(options.output, " > Error: %s\n",
					unw_strerror(rc));
				goto cont;
			}

			/* We are looking for the library with the base address
			 * closest to the current ip.  */
			lib_name = "unmapped_area";
			distance = (size_t) -1;
			lib = proc->libraries;
			while (lib != NULL) {
				/* N.B.: Assumes sizeof(size_t) ==
				 * sizeof(arch_addr_t).
				 * Keyword: double cast.  */
				if ((ip >= lib->base) &&
					    ((size_t)(ip - lib->base)
					    < distance)) {
					distance = ip - lib->base;
					lib_name = lib->pathname;
				}
				lib = lib->next;
			}

			rc = unw_get_proc_name(&cursor, fn_name,
					       sizeof(fn_name),
					       (unw_word_t *) &function_offset);
			if (rc == 0 || rc == -UNW_ENOMEM)
				fprintf(options.output, " > %s(%s+%p) [%p]\n",
					lib_name, fn_name, function_offset, ip);
			else
				fprintf(options.output, " > %s(??\?) [%p]\n",
					lib_name, ip);

		cont:
			if (unw_step(&cursor) <= 0)
				break;
			unwind_depth--;
		}
		fprintf(options.output, "\n");
	}
#endif /* defined(HAVE_LIBUNWIND) */

#if defined(HAVE_LIBDW)
	if (options.bt_depth > 0 && proc->leader->dwfl != NULL) {
		int frames = options.bt_depth;
		if (dwfl_getthread_frames(proc->leader->dwfl, proc->pid,
					  frame_callback, &frames) < 0) {
			// Only print an error if we couldn't show anything.
			// Otherwise just show there might be more...
			if (frames == options.bt_depth)
				fprintf(stderr,
					"dwfl_getthread_frames tid %d: %s\n",
					proc->pid, dwfl_errmsg(-1));
			else
				fprintf(options.output, " > [...]\n");
		}
		fprintf(options.output, "\n");
	  }
#endif /* defined(HAVE_LIBDW) */

	current_proc = NULL;
	current_column = 0;
}

int
delim_output(FILE *stream, int *need_delimp,
	     int (*writer)(FILE *stream, void *data),
	     void *data)
{
	int o;

	/* If we don't need a delimiter, then we don't need to go
	 * through a temporary stream.  It's all the same whether
	 * WRITER emits anything or not.  */
	if (!*need_delimp) {
		o = writer(stream, data);

	} else {
		struct memstream ms;
		if (memstream_init(&ms) < 0)
			return -1;
		o = writer(ms.stream, data);
		if (memstream_close(&ms) < 0)
			o = -1;
		if (o > 0 && ((*need_delimp
			       && account_output(&o, fprintf(stream, ", ")) < 0)
			      || fwrite(ms.buf, 1, ms.size, stream) != ms.size))
			o = -1;

		memstream_destroy(&ms);
	}

	if (o < 0)
		return -1;

	*need_delimp = *need_delimp || o > 0;
	return o;
}

int
account_output(int *countp, int c)
{
	if (c > 0)
		*countp += c;
	return c;
}

static void
do_report(const char *filename, unsigned line_no, const char *severity,
	  const char *fmt, va_list args)
{
	char buf[128];
	vsnprintf(buf, sizeof(buf), fmt, args);
	buf[sizeof(buf) - 1] = 0;
	if (filename != NULL)
		output_line(0, "%s:%d: %s: %s",
			    filename, line_no, severity, buf);
	else
		output_line(0, "%s: %s", severity, buf);
}

void
report_error(const char *filename, unsigned line_no, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	do_report(filename, line_no, "error", fmt, args);
	va_end(args);
}

void
report_warning(const char *filename, unsigned line_no, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	do_report(filename, line_no, "warning", fmt, args);
	va_end(args);
}

void
report_global_error(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	do_report(NULL, 0, "error", fmt, args);
	va_end(args);
}
