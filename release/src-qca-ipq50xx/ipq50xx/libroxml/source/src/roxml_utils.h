/**
 * \file roxml_utils.h
 * \brief misc functions required in libroxml
 *
 * (C) Copyright 2015
 * Tristan Lelong <tristan.lelong@libroxml.net>
 *
 * SPDX-Licence-Identifier:     LGPL-2.1+
 * The author added a static linking exception, see License.txt.
 */

#ifndef ROXML_UTILS_H
#define ROXML_UTILS_H

#include <stdlib.h>

#if defined(_WIN32)
#include "roxml_win32_native.h"
#else
#include <pthread.h>
#endif

#if(CONFIG_XML_THREAD_SAFE==0)
ROXML_STATIC_INLINE ROXML_INT unsigned long int roxml_thread_id(node_t *n)
{
	return 0;
}

ROXML_STATIC_INLINE ROXML_INT int roxml_lock_init(node_t *n)
{
	return 0;
}

ROXML_STATIC_INLINE ROXML_INT int roxml_lock_destroy(node_t *n)
{
	return 0;
}

ROXML_STATIC_INLINE ROXML_INT int roxml_lock(node_t *n)
{
	return 0;
}

ROXML_STATIC_INLINE ROXML_INT int roxml_unlock(node_t *n)
{
	return 0;
}
#else /* CONFIG_XML_THREAD_SAFE==1 */
ROXML_STATIC_INLINE ROXML_INT unsigned long int roxml_thread_id(node_t *n)
{
	return pthread_self();
}

ROXML_STATIC_INLINE ROXML_INT int roxml_lock_init(node_t *n)
{
	xpath_tok_table_t *table = (xpath_tok_table_t *)n->priv;
	table->lock = malloc(sizeof(pthread_mutex_t));
	return pthread_mutex_init(table->lock, NULL);
}

ROXML_STATIC_INLINE ROXML_INT int roxml_lock_destroy(node_t *n)
{
	xpath_tok_table_t *table = (xpath_tok_table_t *)n->priv;
	pthread_mutex_destroy(table->lock);
	free(table->lock);
	return 0;
}

ROXML_STATIC_INLINE ROXML_INT int roxml_lock(node_t *n)
{
	xpath_tok_table_t *table;
	while (n->prnt)
		n = n->prnt;
	
	table = (xpath_tok_table_t *)n->priv;
	return pthread_mutex_lock(table->lock);
}

ROXML_STATIC_INLINE ROXML_INT int roxml_unlock(node_t *n)
{
	xpath_tok_table_t *table;
	while (n->prnt)
		n = n->prnt;
	
	table = (xpath_tok_table_t *)n->priv;
	return pthread_mutex_unlock(table->lock);
}
#endif /* CONFIG_XML_THREAD_SAFE */

#ifdef CONFIG_XML_FLOAT
ROXML_STATIC_INLINE ROXML_INT double roxml_strtonum(const char *str, char **end)
{
	return strtod(str, end);
}
#else /* CONFIG_XML_FLOAT */
ROXML_STATIC_INLINE ROXML_INT double roxml_strtonum(const char *str, char **end)
{
	int value = strtol(str, end, 0);

	/* if the value is a float:
	 * it must be considered a number and we floor it
	 */
	if (end && *end && **end == '.')
		strtol(*(end+1), end, 0);

	return value;

}
#endif /* CONFIG_XML_FLOAT */

#endif /* ROXML_UTILS */
