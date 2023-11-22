#ifndef TIMER_IMPL_H
#define TIMER_IMPL_H

#define ATIMER_T	1

#include <libubox/uloop.h>

struct atimer {
	/* uloop timer implementation */
	struct uloop_timeout t;
	struct timeval expires;
};

typedef struct atimer atimer_t;


#endif /* TIMER_IMPL_H */
