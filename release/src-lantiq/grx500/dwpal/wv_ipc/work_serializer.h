/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#ifndef __WAVE_WORK_SERIALIZER__H__
#define __WAVE_WORK_SERIALIZER__H__

typedef struct _work_serializer work_serializer;

typedef int (*work_cb) (work_serializer *s, void *work_obj, void *ctx);
typedef int (*free_cb) (void *work_obj, void *ctx);
typedef int (*cmp_key) (void *work_obj, void *key);

typedef struct _work_ops {
	work_cb work_func;
	free_cb free_func;
	cmp_key cmp_func;
} work_ops_t;

work_serializer * serializer_create(work_ops_t *ops, unsigned num_ops,
				    int start_now);

int serializer_destroy(work_serializer *s);

int serializer_exec_work(work_serializer *s, unsigned id,
			 void *work_obj, void *ctx, int *res,
			 unsigned int timeout_ms);

int serializer_exec_work_async(work_serializer *s, unsigned id,
			       void *work_obj, void *ctx);

int serializer_add_delayed_work(work_serializer *s, unsigned id,
				void *work_obj, void *ctx,
				unsigned int secs, unsigned int msecs);

int serializer_cancel_delayed_work(work_serializer *s, unsigned id,
				   void *key, void *ctx);

int serializer_stop_all_by_ctx(work_serializer *s, void *ctx);

int serializer_is_running(work_serializer *s);

#endif /* __WAVE_WORK_SERIALIZER__H__ */
