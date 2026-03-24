/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

/*
 *
 * Logger
 *
 * Helper macros for generated logging code
 *
 * $Id$
 *
 */
#ifndef __LOGMACRO_MIXINS_H_
#define __LOGMACRO_MIXINS_H_



#ifndef logpkt_memcpy
#define logpkt_memcpy wave_memcpy
#endif

/* We need to have uint32 word alignment LOG messages payload */
#define LOGPKT_WORD_ALIGNMENT

#ifdef  LOGPKT_WORD_ALIGNMENT
#define LOGPKT_ITEM_SIZE(x) ((((x) + 3) >> 2) << 2) /* for 4-bytes word */
#else
#define LOGPKT_ITEM_SIZE(x) (x)
#endif

#define LOGPKT_STRING_SIZE(str)                                   \
     LOGPKT_ITEM_SIZE((sizeof(mtlk_log_event_data_t) + sizeof(mtlk_log_lstring_t) + str##len__))

#define LOGPKT_SCALAR_SIZE(val)                                   \
     LOGPKT_ITEM_SIZE((sizeof(mtlk_log_event_data_t) + sizeof(val)))

#define LOGPKT_MACADDR_SIZE(addr)                                 \
     LOGPKT_ITEM_SIZE((sizeof(mtlk_log_event_data_t) + MAC_ADDR_LENGTH))

#define LOGPKT_IP6ADDR_SIZE(addr)                                 \
     LOGPKT_ITEM_SIZE((sizeof(mtlk_log_event_data_t) + IP6_ADDR_LENGTH))

#define LOGPKT_SIGNAL_STRUCT_SIZE                                 \
    LOGPKT_ITEM_SIZE((sizeof(mtlk_log_signal_t)))

#define LOGPKT_SIGNAL_SIZE(message_size)                          \
     LOGPKT_ITEM_SIZE((sizeof(mtlk_log_event_data_t) + LOGPKT_SIGNAL_STRUCT_SIZE +  message_size))

#define LOGPKT_TLOG_STRUCT_SIZE                                 \
		LOGPKT_ITEM_SIZE((sizeof(mtlk_log_TLOG_t)))

#define LOGPKT_TLOG_SIZE(message_size)							 \
	 LOGPKT_ITEM_SIZE((sizeof(mtlk_log_event_data_t)+ LOGPKT_TLOG_STRUCT_SIZE +  message_size))

#define LOGPKT_EVENT_HDR_SIZE       (sizeof(mtlk_log_event_t))
#define LOGPKT_EVENT_HDR_SIZE_VER2  LOGPKT_EVENT_HDR_SIZE

#define LOGPKT_SIGNAL_HDR_SIZE                                    \
     (sizeof(mtlk_log_event_data_t) + LOGPKT_SIGNAL_STRUCT_SIZE)

#define LOGPKT_PUT_STRING(str)                                    \
  {                                                               \
      ((mtlk_log_event_data_t *) p__)->datatype = LOG_DT_LSTRING; \
      p__ += sizeof(mtlk_log_event_data_t);                       \
      ((mtlk_log_lstring_t *) p__)->len = str##len__;             \
      p__ += sizeof(mtlk_log_lstring_t);                          \
      logpkt_memcpy(p__, str##len__, str, str##len__);            \
      p__ += LOGPKT_ITEM_SIZE(str##len__);                        \
  }

#define LOGPKT_PUT_INT32(value)                                   \
  {                                                               \
      ((mtlk_log_event_data_t *) p__)->datatype = LOG_DT_INT32;   \
      p__ += sizeof(mtlk_log_event_data_t);                       \
      logpkt_memcpy(p__, sizeof(int32), &value, sizeof(int32));   \
      p__ += sizeof(int32);                                       \
  }

#ifdef  LOGPKT_WORD_ALIGNMENT
#define LOGPKT_PUT_INT8(value)                                    \
  {                                                               \
      uint32 __word__ = value;                                    \
      ((mtlk_log_event_data_t *) p__)->datatype = LOG_DT_INT8;    \
      p__ += sizeof(mtlk_log_event_data_t);                       \
      *(uint32 *)p__ = __word__;                                  \
      p__ += sizeof(__word__);                                    \
  }
#else
#define LOGPKT_PUT_INT8(value)                                    \
  {                                                               \
      ((mtlk_log_event_data_t *) p__)->datatype = LOG_DT_INT8;    \
      p__ += sizeof(mtlk_log_event_data_t);                       \
      logpkt_memcpy(p__, sizeof(int8), &value, sizeof(int8));     \
      p__ += sizeof(int8);                                        \
  }
#endif /* LOGPKT_WORD_ALIGNMENT */

#define LOGPKT_PUT_INT64(value)                                   \
  {                                                               \
      ((mtlk_log_event_data_t *) p__)->datatype = LOG_DT_INT64;   \
      p__ += sizeof(mtlk_log_event_data_t);                       \
      logpkt_memcpy(p__, sizeof(int64), &value, sizeof(int64));   \
      p__ += sizeof(int64);                                       \
  }

#define LOGPKT_PUT_PTR(ptr)                                          \
  {                                                                  \
      if (sizeof(void *) == sizeof(uint32)) {                        \
          ((mtlk_log_event_data_t *) p__)->datatype = LOG_DT_INT32;  \
      }                                                              \
      else if (sizeof(void *) == sizeof(uint64)) {                   \
          ((mtlk_log_event_data_t *) p__)->datatype = LOG_DT_INT64;  \
      }                                                              \
      else {                                                         \
          MTLK_ASSERT(!"Invalid pointer size");                      \
      }                                                              \
      p__ += sizeof(mtlk_log_event_data_t);                          \
      logpkt_memcpy(p__, sizeof(void *), &ptr, sizeof(void *));      \
      p__ += sizeof(void *);                                         \
  }

#define LOGPKT_PUT_MACADDR(addr)                                     \
  {                                                                  \
      ((mtlk_log_event_data_t *) p__)->datatype = LOG_DT_MACADDR;    \
      p__ += sizeof(mtlk_log_event_data_t);                          \
      logpkt_memcpy(p__, MAC_ADDR_LENGTH, (void*) addr, MAC_ADDR_LENGTH); \
      p__ += LOGPKT_ITEM_SIZE(MAC_ADDR_LENGTH);                      \
  }

#define LOGPKT_PUT_IP6ADDR(addr)                                     \
  {                                                                  \
      ((mtlk_log_event_data_t *) p__)->datatype = LOG_DT_IP6ADDR;    \
      p__ += sizeof(mtlk_log_event_data_t);                          \
      logpkt_memcpy(p__, IP6_ADDR_LENGTH, (void*) addr, IP6_ADDR_LENGTH); \
      p__ += LOGPKT_ITEM_SIZE(IP6_ADDR_LENGTH);                      \
  }

#define LOGPKT_PUT_SIGNAL(mtlk_log_signal, message)                  \
  {                                                                  \
      ((mtlk_log_event_data_t *) p__)->datatype = LOG_DT_SIGNAL;     \
      p__ += sizeof(mtlk_log_event_data_t);                          \
      logpkt_memcpy(p__, LOGPKT_SIGNAL_STRUCT_SIZE, &mtlk_log_signal, LOGPKT_SIGNAL_STRUCT_SIZE); \
      p__ += LOGPKT_SIGNAL_STRUCT_SIZE;                              \
      logpkt_memcpy(p__, mtlk_log_signal.len, message, mtlk_log_signal.len); \
      p__ += mtlk_log_signal.len;  /* already aligned size */        \
  }

#define LOGPKT_PUT_SIGNAL_HEADER(mtlk_log_signal)			\
  {                                                                     \
      ((mtlk_log_event_data_t *) p__)->datatype = LOG_DT_SIGNAL;	\
      p__ += sizeof(mtlk_log_event_data_t);				\
      logpkt_memcpy(p__, LOGPKT_SIGNAL_STRUCT_SIZE, &mtlk_log_signal, LOGPKT_SIGNAL_STRUCT_SIZE);	\
      p__ += LOGPKT_SIGNAL_STRUCT_SIZE;					\
  }

#define LOGPKT_PUT_SIGNAL_BODY(message, message_size)	\
  {							\
      logpkt_memcpy(p__, message_size, message, message_size);	\
      p__ += message_size;				\
  }

#define LOGPKT_PUT_TLOG(mtlk_log_TLOG, message)                  \
{                                                                  \
    ((mtlk_log_event_data_t *) p__)->datatype = LOG_DT_TLOG;     \
   p__ += sizeof(mtlk_log_event_data_t);                          \
   logpkt_memcpy(p__, LOGPKT_TLOG_STRUCT_SIZE, &mtlk_log_TLOG, LOGPKT_TLOG_STRUCT_SIZE); \
   p__ += LOGPKT_TLOG_STRUCT_SIZE;                              \
   logpkt_memcpy(p__, mtlk_log_TLOG.len, message, mtlk_log_TLOG.len); \
   p__ += mtlk_log_TLOG.len;  /* already aligned size */        \
}

/* static assert (check during compilation stage) */
#define ct_assert(e) {enum { ct_assert_value = 1/(!!(e)) };}

#endif // __LOGMACRO_MIXINS_H_

