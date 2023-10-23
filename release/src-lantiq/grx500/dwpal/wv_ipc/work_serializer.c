/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#include "work_serializer.h"
#include "pthread.h"
#include "linked_list.h"
#include "logs.h"

#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define ASYNC_WORK_LIST_MAX_SIZE	(500)
#define DELAYED_WORK_LIST_MAX_SIZE	(200)

typedef enum {
	SERIALIZER_PAUSED,
	SERIALIZER_RUNNING,
} serializer_state_t;

typedef struct _work_serializer {
	pthread_t thread_id;
	serializer_state_t state;
	int stop;

	work_ops_t *ops;
	unsigned num_ops;

	l_list *work_list;
	l_list *delayed_work_list;
	pthread_mutex_t work_lock;
	pthread_cond_t work_cond;
} work_serializer;

typedef enum {
	WORK_ASYNC,
	WORK_DELAYED,
	WORK_WAITING_FOR_FINISH,
	WORK_DONE,
	WORK_ABANDONED,
	WORK_ABORTED,
} work_state_t;

typedef struct _work {
	void *ctx;
	void *obj;
	unsigned id;

	struct timespec ts;
	work_state_t state;
	int result;
} work_t;

static inline int _is_delayed_work_before(work_t *work, struct timespec *ts)
{
	if (work->ts.tv_sec < ts->tv_sec ||
	    (work->ts.tv_sec == ts->tv_sec && work->ts.tv_nsec < ts->tv_nsec))
		return 1;
	return 0;
}

static inline void _add_time_to_timespec(struct timespec *ts,
					 unsigned int secs,
					 unsigned int msecs)
{
	ts->tv_sec += secs + (msecs / 1000);
	ts->tv_nsec += (msecs % 1000) * 1000000;

	if (ts->tv_nsec >= 1000000000) {
		ts->tv_sec++;
		ts->tv_nsec %= 1000000000;
	}
}

static void* serializer_work_thread(void *obj)
{
	work_serializer *s = (work_serializer*)obj;
	struct timespec ts, *ts_ptr;
	int cw_res = 0, work_res;
	work_t *work = NULL;
	void *ret = NULL;

	if (s == NULL)
		return (void*)-1;

	while (!s->stop) {
		pthread_mutex_lock(&s->work_lock);

		do {

			work = list_pop_front(s->work_list);
			if (work && work->state == WORK_WAITING_FOR_FINISH) {
				pthread_mutex_unlock(&s->work_lock);
				goto do_work;
			} else if (work)
				break;

			clock_gettime(CLOCK_REALTIME, &ts);

			work = list_peek_front(s->delayed_work_list);
			if (work && _is_delayed_work_before(work, &ts)) {
				work = list_pop_front(s->delayed_work_list);
				break;
			} else if (work && work->ts.tv_sec < ts.tv_sec + 10)
				ts_ptr = &work->ts;
			else {
				ts.tv_sec += 10;
				ts_ptr = &ts;
			}

			work = NULL;
			cw_res = pthread_cond_timedwait(&s->work_cond,
							&s->work_lock, ts_ptr);

		} while (cw_res == 0 && !s->stop);

		pthread_mutex_unlock(&s->work_lock);

		if (cw_res != 0 && cw_res != ETIMEDOUT) {
			ELOG("cond timedwait retunred err (cw_res=%d)", cw_res);
			ret = (void*)-1;
			break;
		}

		if (s->stop)
			break;

		if (!work)
			continue;

do_work:
		work_res = s->ops[work->id].work_func(s, work->obj, work->ctx);
		if (s->ops[work->id].free_func)
			s->ops[work->id].free_func(work->obj, work->ctx);

		pthread_mutex_lock(&s->work_lock);

		if (work->state == WORK_WAITING_FOR_FINISH) {
			work->result = work_res;
			work->state = WORK_DONE;
			pthread_cond_broadcast(&s->work_cond);
		} else
			/* This free is done for ASYNC, DELAYED.
			 * And in addition, SYNC work abandoned by the caller.
			 */
			free(work);

		pthread_mutex_unlock(&s->work_lock);

		work = NULL;
	}

	if (work) {
		if (s->ops[work->id].free_func)
			s->ops[work->id].free_func(work->obj, work->ctx);
		free(work);
	}

	return ret;
}

work_serializer * serializer_create(work_ops_t *ops, unsigned num_ops,
				    int start_now)
{
	work_serializer *s;

	if (ops == NULL || num_ops == 0)
		return NULL;

	s = (work_serializer*)malloc(sizeof(work_serializer));
	if (s == NULL)
		return NULL;

	memset(s, 0, sizeof(work_serializer));

	s->num_ops = num_ops;
	s->ops = (work_ops_t*)malloc(num_ops * sizeof(work_ops_t));
	if (s->ops == NULL) {
		free(s);
		return NULL;
	}
	memcpy(s->ops, ops, num_ops * sizeof(work_ops_t));

	s->work_list = list_init();
	s->delayed_work_list = list_init();
	pthread_mutex_init(&s->work_lock, NULL);
	pthread_cond_init(&s->work_cond, NULL);

	if (start_now) {
		s->state = SERIALIZER_RUNNING;
		pthread_create(&s->thread_id, NULL, serializer_work_thread, s);
	} else
		s->state = SERIALIZER_PAUSED;

	return s;
}

int serializer_destroy(work_serializer *s)
{
	work_t *work, *delayed_work;
	work_ops_t *ops;

	if (s == NULL)
		return 1;

	pthread_mutex_lock(&s->work_lock);
	s->stop = 1;
	pthread_cond_signal(&s->work_cond);
	pthread_mutex_unlock(&s->work_lock);
	if (s->state == SERIALIZER_RUNNING)
		pthread_join(s->thread_id, NULL);

	pthread_mutex_destroy(&s->work_lock);
	pthread_cond_destroy(&s->work_cond);

	ops = s->ops;

	while ((work = list_pop_front(s->work_list))) {
		if (ops[work->id].free_func)
			ops[work->id].free_func(work->obj,
						work->ctx);
		free(work);
	}

	while ((delayed_work = list_pop_front(s->delayed_work_list))) {
		if (ops[delayed_work->id].free_func)
			ops[delayed_work->id].free_func(delayed_work->obj,
							delayed_work->ctx);
		free(delayed_work);
	}

	list_free(s->work_list);
	list_free(s->delayed_work_list);
	free(s->ops);
	free(s);

	return 0;
}

static void _fill_basic_work_data(work_t * work, unsigned id, void *work_obj,
				  void *ctx, work_state_t state)
{
	work->id = id;
	work->obj = work_obj;
	work->ctx = ctx;
	work->result = 0;
	work->state = state;
	clock_gettime(CLOCK_REALTIME, &work->ts);
}

static work_t * _create_new_work(unsigned id, void *work_obj, void *ctx,
				 work_state_t state)
{
	work_t *work = (work_t*)malloc(sizeof(work_t));
	if (work == NULL)
		return NULL;

	_fill_basic_work_data(work, id, work_obj, ctx, state);
	return work;
}

static int _serializer_exec_work(work_serializer *s, work_t *work, int *res,
				 unsigned int timeout_ms)
{
	int inserted = 0, abandoned = 0;
	int cw_res = 0;
	struct timespec ts;
	int trying_to_insert_again = 0;

again:
	if (trying_to_insert_again)
		usleep(200 * 1000);
	trying_to_insert_again = 1;

	pthread_mutex_lock(&s->work_lock);

	if (work->state == WORK_ASYNC) {
		if (list_get_size(s->work_list) >= ASYNC_WORK_LIST_MAX_SIZE) {
			pthread_mutex_unlock(&s->work_lock);
			goto again;
		}

		if (list_push_back(s->work_list, work))
			goto insert_err;
	} else if (work->state == WORK_DELAYED) {
		if (list_get_size(s->delayed_work_list) >= DELAYED_WORK_LIST_MAX_SIZE) {
			pthread_mutex_unlock(&s->work_lock);
			goto again;
		}

		list_foreach_start(s->delayed_work_list, work_entry, work_t)
		if (_is_delayed_work_before(work, &work_entry->ts)) {
			if (list_foreach_insert_before_current(work))
				goto insert_err;
			inserted = 1;
			break;
		}
		list_foreach_end
		if (!inserted && list_push_back(s->delayed_work_list, work))
			goto insert_err;
	} else {
		list_foreach_start(s->work_list, work_entry, work_t)
		if (work_entry->state != WORK_WAITING_FOR_FINISH) {
			if (list_foreach_insert_before_current(work))
				goto insert_err;
			inserted = 1;
			break;
		}
		list_foreach_end
		if (!inserted && list_push_back(s->work_list, work))
			goto insert_err;
	}

	if (s->state == SERIALIZER_PAUSED) {
		s->state = SERIALIZER_RUNNING;
		pthread_create(&s->thread_id, NULL, serializer_work_thread, s);
	} else
		pthread_cond_signal(&s->work_cond);

	if (work->state == WORK_ASYNC || work->state == WORK_DELAYED) {
		pthread_mutex_unlock(&s->work_lock);
		return 0;
	}

	clock_gettime(CLOCK_REALTIME, &ts);
	_add_time_to_timespec(&ts, 0, timeout_ms);
	while (cw_res == 0 && work->state == WORK_WAITING_FOR_FINISH) {
		cw_res = pthread_cond_timedwait(&s->work_cond,
						&s->work_lock, &ts);
	}

	if (work->state == WORK_WAITING_FOR_FINISH &&
	    list_remove(s->work_list, work)) {
		/* this means that we reached a timeout, but the work thread
		 * has already taken the work_t object from the list -> we can't
		 * perform free on it.
		 * mark it as ABANDONED so that the work thread will perform the
		 * free once the work is done.
		 */
		work->state = WORK_ABANDONED;
		abandoned = 1;
	}

	pthread_mutex_unlock(&s->work_lock);

	if (abandoned || work->state == WORK_WAITING_FOR_FINISH) {
		ELOG("sync task err or timeout");
		if (!abandoned)
			free(work);
		return 1;
	}

	if (work->state == WORK_ABORTED) {
		ELOG("sync task was aborted");
		free(work);
		return 1;
	}

	*res = work->result;
	free(work);
	return 0;

insert_err:
	pthread_mutex_unlock(&s->work_lock);
	free(work);
	return 1;
}

int serializer_exec_work(work_serializer *s, unsigned id,
			 void *work_obj, void *ctx, int *res,
			 unsigned int timeout_ms)
{
	work_t *work;

	if (s == NULL || s->num_ops <= id)
		return 1;

	if (s->thread_id == pthread_self()) {
		ELOG("this function can't run from serializer ctx");
		return 1;
	}

	work = _create_new_work(id, work_obj, ctx, WORK_WAITING_FOR_FINISH);
	if (work == NULL)
		return 1;

	return _serializer_exec_work(s, work, res, timeout_ms);
}

int serializer_exec_work_async(work_serializer *s, unsigned id,
			       void *work_obj, void *ctx)
{
	work_t *work;

	if (s == NULL || s->num_ops <= id)
		return 1;

	work = _create_new_work(id, work_obj, ctx, WORK_ASYNC);
	if (work == NULL)
		return 1;

	return _serializer_exec_work(s, work, NULL, 0);
}

int serializer_add_delayed_work(work_serializer *s, unsigned id,
				void *work_obj, void *ctx,
				unsigned int secs, unsigned int msecs)
{
	work_t *work;

	if (s == NULL || s->num_ops <= id)
		return 1;

	work = _create_new_work(id, work_obj, ctx, WORK_DELAYED);
	if (work == NULL)
		return 1;

	_add_time_to_timespec(&work->ts, secs, msecs);

	return _serializer_exec_work(s, work, NULL, 0);
}

int serializer_cancel_delayed_work(work_serializer *s, unsigned id,
				   void *key, void *ctx)
{
	if (s == NULL || s->num_ops <= id)
		return 1;

	if (s->thread_id != pthread_self()) {
		ELOG("this function must run from serializer ctx");
		return 1;
	}

	pthread_mutex_lock(&s->work_lock);

	list_foreach_start(s->delayed_work_list, work, work_t)
		if (id == work->id && ctx == work->ctx &&
		    (!s->ops[id].cmp_func ||
		     !s->ops[id].cmp_func(work->obj, key))) {
			list_foreach_remove_current_entry();
			if (s->ops[id].free_func)
				s->ops[id].free_func(work->obj, work->ctx);
			free(work);
		}
	list_foreach_end

	pthread_mutex_unlock(&s->work_lock);

	return 0;
}

int serializer_stop_all_by_ctx(work_serializer *s, void *ctx)
{
	l_list *list;
	int need_notify = 0;

	if (s == NULL)
		return 1;

	if (s->thread_id != pthread_self()) {
		ELOG("this function must run from serializer ctx");
		return 1;
	}

	list = s->work_list;
	pthread_mutex_lock(&s->work_lock);

again:
	list_foreach_start(s->work_list, work, work_t)
		if (ctx == work->ctx) {
			list_foreach_remove_current_entry();
			if (work->state == WORK_WAITING_FOR_FINISH) {
				work->state = WORK_ABORTED;
				need_notify = 1;
			}
			if (s->ops[work->id].free_func)
				s->ops[work->id].free_func(work->obj, work->ctx);
			if (work->state != WORK_WAITING_FOR_FINISH)
				free(work);
		}
	list_foreach_end
	if (list == s->work_list) {
		list = s->delayed_work_list;
		goto again;
	}

	if (need_notify)
		pthread_cond_broadcast(&s->work_cond);

	pthread_mutex_unlock(&s->work_lock);

	return 0;
}

int serializer_is_running(work_serializer *s)
{
	if (s == NULL)
		return 0;

	return s->state == SERIALIZER_RUNNING;
}
