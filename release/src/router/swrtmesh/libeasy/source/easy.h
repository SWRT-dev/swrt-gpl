#ifndef EASY_H
#define EASY_H

#if __GNUC__ >= 4
#define LIBEASY_API	__attribute__((visibility("default")))
#else
#define LIBEASY_API
#endif

#include "event.h"
#include "utils.h"
#include "if_utils.h"
#include "debug.h"
#include "hlist.h"

#define libeasy_err(...)	pr_error("libeasy: " __VA_ARGS__)
#define libeasy_warn(...)	pr_warn("libeasy: " __VA_ARGS__)
#define libeasy_info(...)	pr_info("libeasy: " __VA_ARGS__)
#define libeasy_dbg(...)	pr_debug("libeasy: " __VA_ARGS__)

#endif /* EASY_H */
