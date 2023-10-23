/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#include "mtlkinc.h"

#include "mtlkmsg.h"
#include "mtlkaux.h"
#include "mtlkhal.h"
#include "txmm.h"

#define LOG_LOCAL_GID   GID_TXMM
#define LOG_LOCAL_FID   1

/* 
   Here you can redefine debug level of all the
   printouts from this module. In order to move all
   printouts to DLevel N just define ILOG2 as LOG<N>.
*/
#define TXMM_FLAGS_ACTIVE              0x40000000
#define TXMM_FLAGS_STOPPED             0x20000000

struct _mtlk_txmm_block_t
{
  mtlk_txmm_data_t                data; /* must be 1st data member */
  PMSG_OBJ                        slot;
  mtlk_txmm_clb_f                 clb;
  mtlk_handle_t                   clb_data;
  mtlk_txmm_block_e               que;
  uint32                          idx;
  mtlk_osal_msec_t                send_time;
  uint32                          timeout;
  mtlk_txmm_msg_t                *msg;
  mtlk_vap_handle_t               vap_handle;
#if CPTCFG_IWLWAV_TXMM_HISTORY_LENGTH
  mtlk_txmm_stats_history_entry_t history;
#endif
};

/************************************************************
 * Memory guards defines
 ************************************************************/
#define MTLK_TXMM_FGUARD_SIZE    sizeof(uint32)
#define MTLK_TXMM_BGUARD_SIZE    sizeof(uint32)
#define MTLK_TXMM_FGUARD_PATTERN 0xBABABABA
#define MTLK_TXMM_BGUARD_PATTERN 0xABABABAB

/**************************************************************************
 * NOTE: actually the payload buffer just follows the header, so this is 
 * equal to: 
 * #define TXMM_BLOCK_PAYLOAD(b) (char*)(b)) + sizeof(*(b))
***************************************************************************/
#define TXMM_BLOCK_PAYLOAD(b) ( (uint8*)&(b)[1] + MTLK_TXMM_FGUARD_SIZE)
/**************************************************************************/

/************************************************************
 * Module-specific result codes
 ************************************************************/
#define MTLK_TXMM_OK_HANDLED (MTLK_ERR_OK)
#define MTLK_TXMM_OK_SENT    (MTLK_ERR_OK + 1)
#define MTLK_TXMM_OK_QUEUED  (MTLK_ERR_OK + 2)
#define MTLK_TXMM_OK_FREED   (MTLK_ERR_OK + 3)
/************************************************************/

static void _mtlk_txmm_init_guard (mtlk_txmm_base_t* obj, mtlk_txmm_block_t* block)
{
  uint8 *fguard = ((uint8 *)block->data.payload) - MTLK_TXMM_FGUARD_SIZE;
  uint8 *bguard = ((uint8 *)block->data.payload) + obj->cfg.max_payload_size;

  *((uint32 *)fguard) = MTLK_TXMM_FGUARD_PATTERN;
  *((uint32 *)bguard) = MTLK_TXMM_BGUARD_PATTERN;
}

static void _mtlk_txmm_check_guard (mtlk_txmm_base_t* obj, mtlk_txmm_block_t* block)
{
  uint8 *fguard = ((uint8 *)block->data.payload) - MTLK_TXMM_FGUARD_SIZE;
  uint8 *bguard = ((uint8 *)block->data.payload) + obj->cfg.max_payload_size;
  
  MTLK_UNREFERENCED_PARAM(fguard);
  MTLK_UNREFERENCED_PARAM(bguard);

  MTLK_ASSERT(*((uint32 *)fguard) == MTLK_TXMM_FGUARD_PATTERN);
  MTLK_ASSERT(*((uint32 *)bguard) == MTLK_TXMM_BGUARD_PATTERN);
}

static __INLINE void _mtlk_txmm_adjust_usage_stats(mtlk_txmm_base_t* obj)
{
  uint32 nof_blocks_used = obj->cfg.max_msgs - obj->ques[MTLK_TXMMB_FREE].total;

  if (obj->stats.used_peak < nof_blocks_used) {
    obj->stats.used_peak = nof_blocks_used;
  }
}

static __INLINE void _mtlk_txmm_zero_block(mtlk_txmm_block_t* block)
{
  memset(block, 0, sizeof(*block));
  block->data.payload = TXMM_BLOCK_PAYLOAD(block);
}

#if CPTCFG_IWLWAV_TXMM_HISTORY_LENGTH
static void _mtlk_txmm_block_history_init(mtlk_txmm_block_t* block)
{
  memset(&block->history, 0, sizeof(block->history));

  block->history.ts[MTLK_TXMM_HE_TS_ARRIVED] = mtlk_osal_timestamp();
  block->history.clb_reason                  = MTLK_TXMM_CLBR_LAST;
  block->history.blocked_res                 = -10000;
  block->history.msg_id                      = block->data.id;
}

static void _mtlk_txmm_block_history_on_send_to_fw (mtlk_txmm_block_t* block)
{
  block->history.ts[MTLK_TXMM_HE_TS_SENT_TO_FW] = mtlk_osal_timestamp();
}

static void _mtlk_txmm_block_history_on_cfm_clb (mtlk_txmm_block_t* block, mtlk_txmm_clb_reason_e reason)
{
  block->history.clb_reason                         = reason;
  block->history.ts[MTLK_TXMM_HE_TS_CFM_CLB_CALLED] = mtlk_osal_timestamp();
}

static void _mtlk_txmm_block_history_commit (mtlk_txmm_block_t* block, mtlk_txmm_base_t* obj)
{
  obj->stats.history.entries[obj->stats.history.widx] = block->history;
  INC_WRAP_IDX(obj->stats.history.widx, ARRAY_SIZE(obj->stats.history.entries));
}

static void _mtlk_txmm_block_history_blocked_finalize_and_commit (mtlk_txmm_block_t* block, mtlk_txmm_base_t* obj, int wait_res)
{
  block->history.ts[MTLK_TXMM_HE_TS_BWAIT_FINISHED] = mtlk_osal_timestamp();
  block->history.blocked_res                        = wait_res;

  mtlk_osal_lock_acquire(&obj->lock);
  _mtlk_txmm_block_history_commit(block, obj);
  mtlk_osal_lock_release(&obj->lock);
}

static void _mtlk_txmm_history_print_unsafe (mtlk_txmm_base_t* obj)
{
  uint32 i = 0;
  for (i = 0; i < ARRAY_SIZE(obj->stats.history.entries); i++)
  {
    uint32 j = 0;
    mtlk_txmm_stats_history_entry_t *e = &obj->stats.history.entries[(obj->stats.history.widx + i) % ARRAY_SIZE(obj->stats.history.entries)];
    ELOG_DDDD("MSG[%u]: id=0x%04X clbR=%u bwtR=%d", i, e->msg_id, e->clb_reason, e->blocked_res);
    for (j = 0; j < ARRAY_SIZE(e->ts); j++)
    {
      ELOG_DD("    ts[%u] = %u", j, mtlk_osal_timestamp_to_ms(e->ts[j]));
    }
  }
}
#else /* CPTCFG_IWLWAV_TXMM_HISTORY_LENGTH */

#define _mtlk_txmm_block_history_init(b)
#define _mtlk_txmm_block_history_on_send_to_fw(b)
#define _mtlk_txmm_block_history_on_cfm_clb(b, r)
#define _mtlk_txmm_block_history_commit(b, o)
#define _mtlk_txmm_block_history_blocked_finalize_and_commit(o, b, r)
#define _mtlk_txmm_history_print_unsafe(o)

#endif /* CPTCFG_IWLWAV_TXMM_HISTORY_LENGTH */

static mtlk_txmm_block_t* _mtlk_txmm_get_buf(mtlk_txmm_base_t* obj, mtlk_txmm_block_e que_idx)
{
  mtlk_txmm_block_t* buf = NULL;
  if (que_idx < MTLK_ARRAY_SIZE(obj->ques))
  {
    mtlk_txmm_que_t* que = &obj->ques[que_idx];

    if (que->total != 0)
    {
        buf = que->ptr[que->from];
        INC_WRAP_IDX(que->from, obj->cfg.max_msgs);
        que->total--;
    }
  }
    
  return buf;
}

static void _mtlk_txmm_put_buf(mtlk_txmm_base_t* obj, mtlk_txmm_block_e que_idx, mtlk_txmm_block_t* buf)
{
  if (que_idx < MTLK_ARRAY_SIZE(obj->ques))
  {
    mtlk_txmm_que_t* que = &obj->ques[que_idx];

    buf->que          = que_idx;
    buf->idx          = que->to;
    que->ptr[que->to] = buf;
    INC_WRAP_IDX(que->to, obj->cfg.max_msgs);
    que->total++;
  }
}

static int _mtlk_txmm_del_buf_from_que(mtlk_txmm_base_t* obj, mtlk_txmm_block_e que_idx, mtlk_txmm_block_t* buf, int preserve_order)
{
  int res = -1;
  if (que_idx < MTLK_ARRAY_SIZE(obj->ques))
  {
    mtlk_txmm_que_t* que = &obj->ques[que_idx];
    if (que->ptr[buf->idx] == buf)
    {
      uint32 i = buf->idx;
      if (i != que->from)
      { /* if not first => rearrange queue, else => just increment que->from */
        if (preserve_order)
        { /* order preservation => shift foregoing elements right */
          do
          {
            int prev = i - 1;
            if (prev < 0)
            {
              prev = obj->cfg.max_msgs - 1;
            }

            que->ptr[i]      = que->ptr[prev];
            que->ptr[i]->idx = i;
            i                = prev;
          } while (i != que->from);
        }
        else
        { /* no order preservation => swap with first */
          que->ptr[i]      = que->ptr[que->from]; /* swap block with 1st     */
          que->ptr[i]->idx = i;                   /* set 1st block new index */
        }
      }

      INC_WRAP_IDX(que->from, obj->cfg.max_msgs);
      que->total--;

      buf->que = MTLK_TXMMB_NONE;
      res      = 0;
    }
    else
    {
      ELOG_DDD("deletion of invalid block from queue#%d (%d, %d)",
               que_idx, buf->que, (int)buf->idx);
    }
  }

  return res;
}

static __INLINE void _mtlk_txmm_free_buf(mtlk_txmm_base_t       *obj, 
                                         mtlk_txmm_block_t *buf,
                                         int                del_from_current_queue,
                                         int                preserve_order)
{
  if (buf->que != MTLK_TXMMB_FREE) 
  {
    if (del_from_current_queue)
    {
      _mtlk_txmm_del_buf_from_que(obj, buf->que, buf, preserve_order);
    }
    _mtlk_txmm_block_history_commit(buf, obj);
    _mtlk_txmm_zero_block(buf);
    _mtlk_txmm_put_buf(obj, MTLK_TXMMB_FREE, buf);
  }
}

typedef void (*mtlk_enum_que_f)(mtlk_txmm_base_t* obj, mtlk_txmm_block_t* buf);

static void _mtlk_txmm_enum_que(mtlk_txmm_base_t* obj, mtlk_txmm_block_e que_idx, mtlk_enum_que_f enum_f)
{
  mtlk_txmm_block_t* buf = NULL;
  if (que_idx < MTLK_ARRAY_SIZE(obj->ques))
  {
    mtlk_txmm_que_t* que = &obj->ques[que_idx];
    uint32           i   = 0;
    uint32           idx = que->from;
    
    for (; i < que->total; i++) {
      buf = que->ptr[idx];
      enum_f(obj, buf);
      INC_WRAP_IDX(idx, obj->cfg.max_msgs);
    }
  }
}

#define _mtlk_txmm_block_by_data(d)  ((mtlk_txmm_block_t*)(d))

static void _mtlk_txmm_err_print_que_block(mtlk_txmm_base_t* obj, mtlk_txmm_block_t* block)
{
  ELOG_DD("\tMSG: id = 0x%04x, size = %d",
        block->data.id,
        block->data.payload_size);
}

static __INLINE void _mtlk_txmm_err_print_err_sent_que(mtlk_txmm_base_t* obj)
{
  ELOG_V("Unconfirmed messages:");
  
  mtlk_osal_lock_acquire(&obj->lock);
  _mtlk_txmm_enum_que(obj, MTLK_TXMMB_SENT, _mtlk_txmm_err_print_que_block);
  _mtlk_txmm_history_print_unsafe(obj);
  mtlk_osal_lock_release(&obj->lock);
}

static void _mtlk_txmm_err_print_queues_unsafe(mtlk_txmm_base_t* obj)
{
  ELOG_V("Sent queue contents:");
  _mtlk_txmm_enum_que(obj, MTLK_TXMMB_SENT, _mtlk_txmm_err_print_que_block);
  ELOG_V("Pending queue contents:");
  _mtlk_txmm_enum_que(obj, MTLK_TXMMB_QUEUED, _mtlk_txmm_err_print_que_block);
  _mtlk_txmm_history_print_unsafe(obj);
}

static void _mtlk_txmm_send_block_prepare(mtlk_txmm_base_t* obj, mtlk_txmm_block_t* block, PMSG_OBJ pmsg)
{
  void* payload_buff = MSG_OBJ_PAYLOAD(pmsg);

  if (!block->data.payload_size || 
      block->data.payload_size > obj->cfg.max_payload_size)
  {
    block->data.payload_size = obj->cfg.max_payload_size;
  }

  block->slot = pmsg;
  _mtlk_txmm_block_history_on_send_to_fw(block);
  wave_memcpy(payload_buff, block->data.payload_size, block->data.payload, block->data.payload_size);
  MSG_OBJ_SET_ID(pmsg, block->data.id);

  _mtlk_txmm_put_buf(obj, MTLK_TXMMB_SENT, block);
  obj->stats.nof_sent++;
}

static int _mtlk_txmm_pump(mtlk_txmm_base_t* obj, PMSG_OBJ* _pmsg, mtlk_vap_handle_t *vap_handle, unsigned* status)
{
  int      res  = MTLK_ERR_UNKNOWN;
  PMSG_OBJ pmsg = *_pmsg;

  ILOG4_PD("PUMP: MSG=%p nU=%d", 
       pmsg, (int)obj->ques[MTLK_TXMMB_QUEUED].total);

  if (obj->ques[MTLK_TXMMB_QUEUED].total)
  {
    if (!pmsg)
    {
      pmsg = obj->api.msg_get_from_pool(obj->api.usr_data); /* get message slot*/
    }

    if (pmsg)
    {
      mtlk_txmm_block_t* block = _mtlk_txmm_get_buf(obj, MTLK_TXMMB_QUEUED);

      if (NULL == block) {
        ELOG_V("PUMP: Can't get free buffer");

        obj->api.msg_free_to_pool(obj->api.usr_data, pmsg); /* free message slot */
        pmsg = NULL;

        if (status)
          *status  = MTLK_TXMM_OK_FREED;
      }
      else {
        ILOG4_V("PUMP: msg reusing...");
        _mtlk_txmm_send_block_prepare(obj, block, pmsg);

        *vap_handle = block->vap_handle;

        if (status)
          *status = MTLK_TXMM_OK_SENT; /* msg sent */

        res = MTLK_ERR_OK;
      }
    }
    else
    {
      ILOG4_V("PUMP: No msg to use...");
  
      if(status)  
        *status = MTLK_TXMM_OK_HANDLED;
  
      res = MTLK_ERR_OK;
    }
  }
  else if (pmsg)
  {
    ILOG4_V("PUMP: msg freeing...");
    obj->api.msg_free_to_pool(obj->api.usr_data, pmsg);
    pmsg = NULL;

    if (status)
      *status  = MTLK_TXMM_OK_FREED;
  
    res = MTLK_ERR_OK;
  }
  else
  {
    /* Nothing to send and no slot given */

    if (status)
      *status = MTLK_TXMM_OK_HANDLED;

    res = MTLK_ERR_OK;
  }

  *_pmsg = pmsg;

  return res;
}

/* returns TRUE if block has been re-queued */
static BOOL _mtlk_txmm_call_usr_clb_locked (mtlk_txmm_base_t*      obj, 
                                            mtlk_txmm_block_t*     block,
                                            mtlk_txmm_clb_reason_e reason)
{
  mtlk_txmm_clb_action_e clb_action = MTLK_TXMM_CLBA_FREE;
  BOOL                   re_queued  = FALSE;
  mtlk_txmm_msg_t       *msg        = NULL;

  MTLK_ASSERT(block->msg != NULL);
  msg = block->msg;                   /* we have to store this pointer here, 
                                       * since the block is zeroed on free 
                                       */

  mtlk_osal_lock_acquire(&msg->lock); /* for safe access to msg object */

  if (!msg->data) { /* the msg has been cancelled =>
                     * do nothing, mtlk_txmm_msg_cancel
                     * function frees the msg object.
                     */
    mtlk_osal_lock_release(&msg->lock);
    return re_queued;
  }
  mtlk_osal_lock_release(&msg->lock);

  _mtlk_txmm_block_history_on_cfm_clb(block, reason);
  clb_action = block->clb(block->clb_data, &block->data, reason);

  switch (clb_action) {
  case MTLK_TXMM_CLBA_FREE:
    mtlk_osal_lock_acquire(&obj->lock); /* for safe access to txmm object */
    _mtlk_txmm_free_buf(obj, block, 0, 0);
    mtlk_osal_lock_release(&obj->lock);
    mtlk_osal_lock_acquire(&msg->lock); /* for safe access to msg object */
    msg->data = NULL;
    msg->obj  = NULL;
    mtlk_osal_lock_release(&msg->lock);
    break;
  case MTLK_TXMM_CLBA_SEND:
    mtlk_osal_lock_acquire(&obj->lock); /* for safe access to txmm object */
    _mtlk_txmm_block_history_commit(block, obj);
    if (block->timeout) { /* get timestamp only if timeout required */
      block->send_time = mtlk_osal_timestamp_to_ms(mtlk_osal_timestamp());
    }
    _mtlk_txmm_block_history_init(block);
    _mtlk_txmm_put_buf(obj, MTLK_TXMMB_QUEUED, block);
    re_queued  = TRUE;
    mtlk_osal_lock_release(&obj->lock);
    break;
  case MTLK_TXMM_CLBA_KEEP:
    break;
  case MTLK_TXMM_CLBA_LAST:
  default:
    ELOG_D("Unexpected callback action returned: %d", clb_action);
    MTLK_ASSERT(0);
    break;
  }

  return re_queued;
}


static uint32 _mtlk_txmm_on_timer_que(mtlk_txmm_base_t*           obj, 
                                      mtlk_osal_msec_t       now_time, 
                                      mtlk_txmm_block_e      que_idx, 
                                      mtlk_txmm_clb_reason_e reason, 
                                      int                    preserve_order)
{
  mtlk_txmm_que_t*   que      = NULL;
  mtlk_txmm_block_t* block    = NULL;  
  uint32             i        = 0;
  uint32             ptr_cnt  = 0;

  que = &obj->ques[que_idx];

  mtlk_osal_lock_acquire(&obj->lock);
  i = que->from;
  while (i != que->to)
  {
    block = que->ptr[i];
    if (block->timeout && block->clb)
    {
      mtlk_osal_ms_diff_t diff_time = mtlk_osal_ms_time_diff(now_time, block->send_time);
      if (diff_time >= block->timeout)
      {
        _mtlk_txmm_del_buf_from_que(obj, que_idx, block, preserve_order);
        _mtlk_txmm_put_buf(obj, MTLK_TXMMB_TIMED_OUT, block); 
 
         ++ptr_cnt;
      }
    }
    INC_WRAP_IDX(i, obj->cfg.max_msgs);
  }
  mtlk_osal_lock_release(&obj->lock);

  que = &obj->ques[MTLK_TXMMB_TIMED_OUT];
  for (;;)
  {
    block = NULL;
    mtlk_osal_lock_acquire(&obj->lock);
    if (que->total)
    {
      block = que->ptr[que->from];
      _mtlk_txmm_del_buf_from_que(obj, MTLK_TXMMB_TIMED_OUT, block, 0);
    }
    mtlk_osal_lock_release(&obj->lock);
    if (!block)
    {
      break;
    }

    ELOG_DDDD("send: timeout (id=0x%04x). Res=%d (req=%u cfm=%u)",
          block->data.id,
          reason,
          obj->stats.nof_sent,
          obj->stats.nof_cfmed); 

    obj->api.msg_timed_out(obj->api.usr_data, block->data.id);
    _mtlk_txmm_call_usr_clb_locked(obj, block, reason);
  }

  if (ptr_cnt)  {
    _mtlk_txmm_err_print_err_sent_que(obj);
  }

  return ptr_cnt;
}

static uint32 __MTLK_IFUNC _mtlk_txmm_on_timer(mtlk_osal_timer_t *timer,
                                               mtlk_handle_t     clb_usr_data)
{
  mtlk_txmm_base_t          *obj = HANDLE_T_PTR(mtlk_txmm_base_t, clb_usr_data);
  mtlk_osal_msec_t now_time = mtlk_osal_timestamp_to_ms(mtlk_osal_timestamp());

  _mtlk_txmm_on_timer_que(obj, now_time, MTLK_TXMMB_SENT, MTLK_TXMM_CLBR_TIMEOUT_SENT, 0);
  _mtlk_txmm_on_timer_que(obj, now_time, MTLK_TXMMB_QUEUED, MTLK_TXMM_CLBR_TIMEOUT_UNSENT, 1);

  return obj->cfg.tmr_granularity;
}

static __INLINE int _mtlk_txmm_on_cfm(mtlk_txmm_base_t* obj, PMSG_OBJ pmsg)
{
  int                res   = MTLK_ERR_UNKNOWN; 
  mtlk_txmm_block_t* block = NULL;

  ILOG4_PD("OnCFM: MSG=%p nS=%d", 
       pmsg, (int)obj->ques[MTLK_TXMMB_SENT].total);

  obj->stats.nof_cfmed++;

  mtlk_osal_lock_acquire(&obj->lock);
  if (obj->ques[MTLK_TXMMB_SENT].total)
  { /* there are sent messages */
    mtlk_txmm_que_t*   que = &obj->ques[MTLK_TXMMB_SENT];
    uint32             i   = que->from;
    
    do
    {
      if (que->ptr[i]->slot == pmsg)
      { /* Message found in sent queue */
        ILOG4_D("OnCFM: msg found in SENT queue: IDX=%d", (int)i);
        block = que->ptr[i];
        _mtlk_txmm_del_buf_from_que(obj, MTLK_TXMMB_SENT, block, 0);
        res = MTLK_ERR_OK;
        break;
      }

      INC_WRAP_IDX(i, obj->cfg.max_msgs);
    } while (i != que->to);
  }
  mtlk_osal_lock_release(&obj->lock);

  if (block)
  {
    void *payload = MSG_OBJ_PAYLOAD(pmsg);

    /* we do not copy cfm id back, just payload */
    wave_memcpy(block->data.payload, obj->cfg.max_payload_size, payload, obj->cfg.max_payload_size);

    _mtlk_txmm_call_usr_clb_locked(obj, block, MTLK_TXMM_CLBR_CONFIRMED);
  }

  return res;
}

static mtlk_txmm_clb_action_e __MTLK_IFUNC
_mtlk_txmm_blocked_send_clb (mtlk_handle_t clb_usr_data, mtlk_txmm_data_t* data, mtlk_txmm_clb_reason_e reason)
{
  mtlk_txmm_t* obj = (mtlk_txmm_t*)clb_usr_data;

  MTLK_UNREFERENCED_PARAM(reason);
  mtlk_osal_event_set(&obj->base->bsend.evt);

  return MTLK_TXMM_CLBA_KEEP; /* Do not free message since it has been sent in blocked mode. */
}

MTLK_INIT_STEPS_LIST_BEGIN(txmm)
  MTLK_INIT_STEPS_LIST_ENTRY(txmm, TXMM_LOCK)
  MTLK_INIT_STEPS_LIST_ENTRY(txmm, TXMM_BSEND_MUTEX)
  MTLK_INIT_STEPS_LIST_ENTRY(txmm, TXMM_BSEND_EVENT)
  MTLK_INIT_STEPS_LIST_ENTRY(txmm, TXMM_BSEND_RMLOCK)
  MTLK_INIT_STEPS_LIST_ENTRY(txmm, TXMM_STORAGE)
  MTLK_INIT_STEPS_LIST_ENTRY(txmm, TXMM_QUEUES)
  MTLK_INIT_STEPS_LIST_ENTRY(txmm, TXMM_TIMER)
MTLK_INIT_INNER_STEPS_BEGIN(txmm)
MTLK_INIT_STEPS_LIST_END(txmm);

MTLK_START_STEPS_LIST_BEGIN(txmm)
  MTLK_START_STEPS_LIST_ENTRY(txmm, TXMM_TIMER)
MTLK_START_INNER_STEPS_BEGIN(txmm)
MTLK_START_STEPS_LIST_END(txmm);

int __MTLK_IFUNC mtlk_txmm_init(mtlk_txmm_base_t           *obj, 
                                const mtlk_txmm_cfg_t      *cfg, 
                                const mtlk_txmm_wrap_api_t *api)
{
  uint32 i          = 0;
  uint32 block_size = 0;
  void  *buf        = NULL;

  MTLK_ASSERT(cfg != NULL);
  MTLK_ASSERT(cfg->max_payload_size != 0);
  MTLK_ASSERT(cfg->max_msgs != 0);
  MTLK_ASSERT(cfg->tmr_granularity != 0);
  MTLK_ASSERT(api != NULL);
  MTLK_ASSERT(api->msg_get_from_pool != NULL);
  MTLK_ASSERT(api->msg_free_to_pool != NULL);
  MTLK_ASSERT(api->msg_send != NULL);

  memset(obj, 0, sizeof(*obj));

  obj->flags |= TXMM_FLAGS_STOPPED;

  block_size = (uint32)sizeof(mtlk_txmm_block_t) + MTLK_TXMM_FGUARD_SIZE + cfg->max_payload_size + MTLK_TXMM_BGUARD_SIZE;

  MTLK_INIT_TRY(txmm, MTLK_OBJ_PTR(obj))
    MTLK_INIT_STEP(txmm, TXMM_LOCK, MTLK_OBJ_PTR(obj), 
                   mtlk_osal_lock_init,  (&obj->lock));
    MTLK_INIT_STEP(txmm, TXMM_BSEND_MUTEX, MTLK_OBJ_PTR(obj), 
                   mtlk_osal_mutex_init, (&obj->bsend.mutex));
    MTLK_INIT_STEP(txmm, TXMM_BSEND_EVENT, MTLK_OBJ_PTR(obj),
                   mtlk_osal_event_init, (&obj->bsend.evt));
    MTLK_INIT_STEP(txmm, TXMM_BSEND_RMLOCK,MTLK_OBJ_PTR(obj),
                   mtlk_rmlock_init, (&obj->bsend.rmlock));

    MTLK_INIT_STEP_EX(txmm, TXMM_STORAGE, MTLK_OBJ_PTR(obj), 
                      mtlk_osal_mem_alloc, (block_size * cfg->max_msgs, MTLK_MEM_TAG_TXMM_STORAGE),
                      buf,
                      buf != NULL,
                      MTLK_ERR_NO_MEM);
    obj->storage = (uint8 *)buf;

    for (i = 0; i < MTLK_ARRAY_SIZE(obj->ques); i++) {
      MTLK_INIT_STEP_LOOP_EX(txmm, TXMM_QUEUES, MTLK_OBJ_PTR(obj),
                             mtlk_osal_mem_alloc, (cfg->max_msgs * sizeof(mtlk_txmm_block_t*), MTLK_MEM_TAG_TXMM_QUEUE),
                             buf,
                             buf != NULL,
                             MTLK_ERR_NO_MEM);
      obj->ques[i].ptr = (mtlk_txmm_block_t**)buf;
    }

    obj->cfg = *cfg;
    obj->api = *api;

    MTLK_INIT_STEP(txmm, TXMM_TIMER, MTLK_OBJ_PTR(obj),
                   mtlk_osal_timer_init, (&obj->timer, _mtlk_txmm_on_timer, HANDLE_T(obj)));

    for (i = 0; i < obj->cfg.max_msgs; i++)
    {
      mtlk_txmm_block_t* block = (mtlk_txmm_block_t*)(obj->storage + (i * block_size));

      /* Init block data payload pointer */
      block->data.payload = TXMM_BLOCK_PAYLOAD(block);

      /* All the messages initially belong to the FREE queue */
      block->que          = MTLK_TXMMB_FREE;
      block->idx          = i;

      _mtlk_txmm_init_guard(obj, block);

      obj->ques[MTLK_TXMMB_FREE].ptr[i] = block;
      obj->ques[MTLK_TXMMB_FREE].total++;
    }

    mtlk_rmlock_acquire(&obj->bsend.rmlock);

  MTLK_INIT_FINALLY(txmm, MTLK_OBJ_PTR(obj))
  MTLK_INIT_RETURN(txmm, MTLK_OBJ_PTR(obj), mtlk_txmm_cleanup, (obj))
}

int __MTLK_IFUNC mtlk_txmm_start(mtlk_txmm_base_t *obj)
{
  MTLK_START_TRY(txmm, MTLK_OBJ_PTR(obj))
  
  MTLK_START_STEP(txmm, TXMM_TIMER, MTLK_OBJ_PTR(obj),
                    mtlk_osal_timer_set, (&obj->timer,
                                          obj->cfg.tmr_granularity));

  obj->flags |= TXMM_FLAGS_ACTIVE;  /* allow blocked send from this point */
  obj->flags &= ~TXMM_FLAGS_STOPPED;

  MTLK_START_FINALLY(txmm, MTLK_OBJ_PTR(obj))
  MTLK_START_RETURN(txmm, MTLK_OBJ_PTR(obj), mtlk_txmm_stop, (obj))
}

void __MTLK_IFUNC
mtlk_txmm_restore(mtlk_txmm_base_t *obj)
{
  unsigned i;
  int block_size;

  block_size = (uint32)sizeof(mtlk_txmm_block_t) + MTLK_TXMM_FGUARD_SIZE + obj->cfg.max_payload_size + MTLK_TXMM_BGUARD_SIZE;

  /* Move all TXMM message to FREE queue */
  for (i = 0; i < obj->cfg.max_msgs; i++)
  {
    mtlk_txmm_block_t* block = (mtlk_txmm_block_t*)(obj->storage + (i * block_size));

    if (block->que == MTLK_TXMMB_SENT || block->que == MTLK_TXMMB_QUEUED) {
      _mtlk_txmm_call_usr_clb_locked(obj, block, MTLK_TXMM_CLBR_RCVRY_DISCARD);
    }
    _mtlk_txmm_free_buf(obj, block, 1, 0);
  }

  for (i = 0; i < MTLK_ARRAY_SIZE(obj->ques); i++)
  {
    ILOG2_DDDD("TXMM sanity check queue #%d: To=%d, From=%d, Total=%d",
      i,
      obj->ques[i].to,
      obj->ques[i].from,
      obj->ques[i].total);

    if (i != MTLK_TXMMB_FREE)
    {
      MTLK_ASSERT(obj->ques[i].from == obj->ques[i].to);
      MTLK_ASSERT(obj->ques[i].total == 0);
    }
    else
    {
      MTLK_ASSERT(obj->ques[i].from == obj->ques[i].to);
      MTLK_ASSERT(obj->ques[i].total == obj->cfg.max_msgs);
    }
  }
}

static mtlk_txmm_data_t * _mtlk_txmm_attach_data (mtlk_txmm_t *obj, mtlk_txmm_msg_t *msg)
{
  mtlk_txmm_block_t* block = NULL;

  mtlk_osal_lock_acquire(&obj->base->lock);
  if (!(obj->base->flags & TXMM_FLAGS_ACTIVE)) {
    mtlk_osal_lock_release(&obj->base->lock);
    return NULL;
  }

  block = _mtlk_txmm_get_buf(obj->base, MTLK_TXMMB_FREE);/* extract slot from free nodes pool */
  mtlk_osal_lock_release(&obj->base->lock);

  if (block) {
    mtlk_osal_lock_acquire(&obj->base->lock);
    _mtlk_txmm_adjust_usage_stats(obj->base);
    mtlk_osal_lock_release(&obj->base->lock);

    block->msg = msg;
    block->que = MTLK_TXMMB_NONE; /* mark block as at user responsibility */
    mtlk_osal_lock_acquire(&msg->lock);
    msg->obj   = obj;
    msg->data  = &block->data;
    mtlk_osal_lock_release(&msg->lock);
  }
  else {
    ELOG_V("No free slot available.");
    mtlk_osal_lock_acquire(&obj->base->lock);
    _mtlk_txmm_err_print_queues_unsafe(obj->base);
    mtlk_osal_lock_release(&obj->base->lock);
    mtlk_osal_lock_acquire(&msg->lock);
    msg->obj   = NULL;
    msg->data  = NULL;
    mtlk_osal_lock_release(&msg->lock);
  }

  return msg->data;
}

static int _mtlk_txmm_send_ex (mtlk_txmm_t*      obj, 
                               mtlk_txmm_data_t* data,
                               mtlk_txmm_clb_f   clb,
                               mtlk_handle_t     clb_usr_data,
                               uint32            timeout_ms)
{
  int                res   = MTLK_ERR_UNKNOWN;
  mtlk_txmm_block_t* block = _mtlk_txmm_block_by_data(data);
  PMSG_OBJ           pmsg;
  mtlk_vap_handle_t  vap_handle = obj->vap_handle;

  MTLK_ASSERT(clb != NULL);

  if (
      data->id != UM_MAN_GET_STATISTICS_REQ &&
      data->id != UM_MAN_POLL_CLIENT_REQ
      )
  {
      ILOG2_DDD("CID-%04x: SEND Id 0x%04X, len %d",
                mtlk_vap_get_oid(vap_handle), data->id, data->payload_size);

      mtlk_dump(2, data->payload, MIN(16, data->payload_size), "Payload");
  }

  _mtlk_txmm_check_guard(obj->base, block);

  block->slot       = NULL;
  block->clb        = clb;
  block->clb_data   = clb_usr_data;
  block->timeout    = timeout_ms;
  block->vap_handle = vap_handle;

  _mtlk_txmm_block_history_init(block);

  if (timeout_ms) /* get timestamp only if timeout required */
    block->send_time = mtlk_osal_timestamp_to_ms(mtlk_osal_timestamp());

  mtlk_osal_lock_acquire(&obj->base->lock);
  pmsg  = obj->base->api.msg_get_from_pool(obj->base->api.usr_data); /* get message slot */

  if (!obj->base->ques[MTLK_TXMMB_QUEUED].total && pmsg)
  { /* no nodes queued and message slot available => direct send message by block */
    ILOG4_P("SEND: direct block sending: MSG=%p", pmsg);
    _mtlk_txmm_send_block_prepare(obj->base, block, pmsg);   /* prepare block, also put it to TXMMB_SENT queue */
    res = MTLK_ERR_OK;
  }
  else
  { /* node queueing */
    ILOG4_V("SEND: block queued");
    _mtlk_txmm_put_buf(obj->base, MTLK_TXMMB_QUEUED, block);

    if (pmsg)
      /* message slot available */
      _mtlk_txmm_pump(obj->base, &pmsg, &vap_handle, NULL); /* pump, i.e. prepare to send 1st block queued */

    res = MTLK_ERR_OK;
  }
  mtlk_osal_lock_release(&obj->base->lock);

  if (pmsg)
    obj->base->api.msg_send(obj->base->api.usr_data, pmsg, vap_handle);

  return res;
}

static int _mtlk_txmm_send_blocked(mtlk_txmm_t*      obj, 
                                   mtlk_txmm_data_t* data,
                                   uint32            timeout_ms)
{
  int res = MTLK_ERR_UNKNOWN;
  mtlk_vap_handle_t master_vap;
  BOOL is_pending = FALSE;

  ASSERT(obj->base->flags & TXMM_FLAGS_ACTIVE); /* Nobody should call this function after the cleanup */

  res = mtlk_vap_manager_get_master_vap(mtlk_vap_get_manager(obj->vap_handle), &master_vap);
  if (res != MTLK_ERR_OK) {
    return res;
  }

  res = mtlk_vap_get_core_vft(master_vap)->get_prop(master_vap,
                                                    MTLK_CORE_PROP_IS_MAC_FATAL_PENDING,
                                                    &is_pending,
                                                    sizeof(is_pending));
  if (res != MTLK_ERR_OK) {
    return MTLK_ERR_PARAMS;
  }

  if (is_pending) {
    return MTLK_ERR_NOT_READY;
  }
  
  mtlk_rmlock_acquire(&obj->base->bsend.rmlock);
  mtlk_osal_mutex_acquire(&obj->base->bsend.mutex);

  if ((obj->base->flags & TXMM_FLAGS_ACTIVE) == 0)
  { /* Cleanup issued. We must release all queued by mtlk_osal_mutex_acquire() threads */
    ILOG2_V("send (b): cleaning up - ignored");
    goto FINISH;
  }

  mtlk_osal_event_reset(&obj->base->bsend.evt);

  res = _mtlk_txmm_send_ex(obj,
                           data,
                           _mtlk_txmm_blocked_send_clb,
                           HANDLE_T(obj),
                           0);

  if (res != MTLK_ERR_OK)
  {
    ELOG_D("send (b): send failed. Err#%d", res);
    goto FINISH;
  }

  ILOG4_D("send (b): wait event (%d ms)", (int)timeout_ms);
  res = mtlk_osal_event_wait(&obj->base->bsend.evt, timeout_ms);
  
  _mtlk_txmm_block_history_blocked_finalize_and_commit(_mtlk_txmm_block_by_data(data), obj->base, res);

  if (res != MTLK_ERR_OK)
  {
    obj->base->api.msg_timed_out(obj->base->api.usr_data, data->id);
    ELOG_DDDD("send (b): wait failed (id=0x%04x). Err#%d (req=%u cfm=%u)", 
          data->id,
          res, 
          obj->base->stats.nof_sent, 
          obj->base->stats.nof_cfmed);
    _mtlk_txmm_err_print_err_sent_que(obj->base);
    goto FINISH;
  }
  else if ((obj->base->flags & TXMM_FLAGS_ACTIVE) == 0) 
  {
    ILOG4_V("send (b): cancelled by cleanup");
    goto FINISH;
  }
  
  res = MTLK_ERR_OK;

FINISH: 
  mtlk_osal_mutex_release(&obj->base->bsend.mutex);
  mtlk_rmlock_release(&obj->base->bsend.rmlock);
  return res;
}

static void _mtlk_txmm_free(mtlk_txmm_base_t* obj, mtlk_txmm_data_t* data, mtlk_txmm_msg_t *msg)
{
  mtlk_txmm_block_t* block = _mtlk_txmm_block_by_data(data);
  mtlk_osal_lock_release(&msg->lock);

  switch (block->que)
  {
  case MTLK_TXMMB_FREE:
    ELOG_D("Second attempt to free msg (id=0x%04x)!", data->id);
    break;
  case MTLK_TXMMB_QUEUED:
    mtlk_osal_lock_acquire(&obj->lock);
    _mtlk_txmm_free_buf(obj, block, 1, 1);
    mtlk_osal_lock_release(&obj->lock);
    break;
  case MTLK_TXMMB_SENT:
    mtlk_osal_lock_acquire(&obj->lock);
    _mtlk_txmm_free_buf(obj, block, 1, 0);
    mtlk_osal_lock_release(&obj->lock);
    break;
  case MTLK_TXMMB_NONE:
  default:
    mtlk_osal_lock_acquire(&obj->lock);
    _mtlk_txmm_free_buf(obj, block, 0, 0);
    mtlk_osal_lock_release(&obj->lock);
    break;
  }
}

int __MTLK_IFUNC mtlk_txmm_pump(mtlk_txmm_base_t* obj, PMSG_OBJ pmsg)
{
  int res;
  unsigned status;
  mtlk_vap_handle_t vap_handle;

  mtlk_osal_lock_acquire(&obj->lock);
  res = _mtlk_txmm_pump(obj, &pmsg, &vap_handle, &status);
  mtlk_osal_lock_release(&obj->lock);

  if (status == MTLK_TXMM_OK_SENT)
    obj->api.msg_send(obj->api.usr_data, pmsg, vap_handle);

  return res;
}

int __MTLK_IFUNC mtlk_txmm_on_cfm(mtlk_txmm_base_t* obj, PMSG_OBJ pmsg)
{
  int res = MTLK_ERR_UNKNOWN; 

  if (pmsg)
  {
    res = _mtlk_txmm_on_cfm(obj, pmsg);
  }
  else
  {
    ELOG_V("NULL message in on_cfm");
    res = MTLK_ERR_PARAMS; 
  }

  return res;
}

void __MTLK_IFUNC mtlk_txmm_stop(mtlk_txmm_base_t *obj)
{
  if (obj->flags & TXMM_FLAGS_STOPPED) {
    return;
  }

  MTLK_STOP_BEGIN(txmm, MTLK_OBJ_PTR(obj))
    MTLK_STOP_STEP(txmm, TXMM_TIMER, MTLK_OBJ_PTR(obj),
                   mtlk_osal_timer_cancel_sync, (&obj->timer));
    obj->flags |= TXMM_FLAGS_STOPPED;
  MTLK_STOP_END(txmm, MTLK_OBJ_PTR(obj))
}

void __MTLK_IFUNC mtlk_txmm_cleanup(mtlk_txmm_base_t* obj)
{
  int i = 0;

  if (obj->flags & TXMM_FLAGS_ACTIVE) {
    obj->flags &= ~TXMM_FLAGS_ACTIVE;        /* Prevent further blocked send waits        */
    mtlk_osal_event_set(&obj->bsend.evt);    /* Cancel current blocked send wait if fired */
    mtlk_rmlock_release(&obj->bsend.rmlock); /* Release last reference to remove lock     */
  
    ILOG2_V("Wait until all the blocked sends are done...");
    i = mtlk_rmlock_wait(&obj->bsend.rmlock);/* Wait until all the blocked sends are done */
    ILOG2_D("Wait finished (res = %d)", i);
    ASSERT(i == MTLK_ERR_OK);
  }

  MTLK_CLEANUP_BEGIN(txmm, MTLK_OBJ_PTR(obj))

    MTLK_CLEANUP_STEP(txmm, TXMM_TIMER, MTLK_OBJ_PTR(obj),
                      mtlk_osal_timer_cleanup, (&obj->timer));

    ILOG0_DD("Usage Peak=%u (Total=%u)", obj->stats.used_peak, obj->cfg.max_msgs);

    for (i = 0; MTLK_CLEANUP_ITERATONS_LEFT(MTLK_OBJ_PTR(obj), TXMM_QUEUES) > 0; i++) {
      MTLK_ASSERT(NULL != obj->ques[i].ptr);
      MTLK_CLEANUP_STEP_LOOP(txmm, TXMM_QUEUES, MTLK_OBJ_PTR(obj),
                             mtlk_osal_mem_free, (obj->ques[i].ptr));
    }

    MTLK_CLEANUP_STEP(txmm, TXMM_STORAGE, MTLK_OBJ_PTR(obj),
                      mtlk_osal_mem_free, (obj->storage));
    MTLK_CLEANUP_STEP(txmm, TXMM_BSEND_RMLOCK, MTLK_OBJ_PTR(obj),
                      mtlk_rmlock_cleanup, (&obj->bsend.rmlock));
    MTLK_CLEANUP_STEP(txmm, TXMM_BSEND_EVENT, MTLK_OBJ_PTR(obj),
                      mtlk_osal_event_cleanup, (&obj->bsend.evt));
    MTLK_CLEANUP_STEP(txmm, TXMM_BSEND_MUTEX, MTLK_OBJ_PTR(obj),
                      mtlk_osal_mutex_cleanup, (&obj->bsend.mutex));
    MTLK_CLEANUP_STEP(txmm, TXMM_LOCK, MTLK_OBJ_PTR(obj),
                      mtlk_osal_lock_cleanup, (&obj->lock));
  MTLK_CLEANUP_END(txmm, MTLK_OBJ_PTR(obj))

  memset(obj, 0, sizeof(*obj));
}

void __MTLK_IFUNC mtlk_txmm_halt(mtlk_txmm_base_t* obj)
{
  obj->flags &= ~TXMM_FLAGS_ACTIVE;
}

BOOL __MTLK_IFUNC mtlk_txmm_is_running(mtlk_txmm_base_t* obj)
{
  return (BOOL) ((obj->flags & TXMM_FLAGS_ACTIVE) == TXMM_FLAGS_ACTIVE);
}

MTLK_INIT_STEPS_LIST_BEGIN(txmm_msg)
  MTLK_INIT_STEPS_LIST_ENTRY(txmm_msg, TXMM_MSG_LOCK)
  MTLK_INIT_STEPS_LIST_ENTRY(txmm_msg, TXMM_MSG_DATA)
#ifdef CPTCFG_IWLWAV_ENABLE_OBJPOOL
  MTLK_INIT_STEPS_LIST_ENTRY(txmm_msg, TXMM_MSG_OBJPOOL)
#endif
MTLK_INIT_INNER_STEPS_BEGIN(txmm_msg)
MTLK_INIT_STEPS_LIST_END(txmm_msg);

int __MTLK_IFUNC
#ifndef CPTCFG_IWLWAV_ENABLE_OBJPOOL
mtlk_txmm_msg_init (mtlk_txmm_msg_t *msg)
#else
_mtlk_txmm_msg_init_objpool(mtlk_txmm_msg_t *msg,
                            mtlk_slid_t caller_slid)
#endif
{
  MTLK_ASSERT(msg != NULL);

  msg->data = NULL;
  msg->obj  = NULL;

  MTLK_INIT_TRY(txmm_msg, MTLK_OBJ_PTR(msg))
    MTLK_INIT_STEP(txmm_msg, TXMM_MSG_LOCK, MTLK_OBJ_PTR(msg), 
                   mtlk_osal_lock_init,  (&msg->lock));
    MTLK_INIT_STEP_VOID(txmm_msg, TXMM_MSG_DATA, MTLK_OBJ_PTR(msg), 
                        MTLK_NOACTION,  ());
#ifdef CPTCFG_IWLWAV_ENABLE_OBJPOOL
    MTLK_INIT_STEP_VOID(txmm_msg, TXMM_MSG_OBJPOOL, MTLK_OBJ_PTR(msg), 
                        mtlk_objpool_add_object, (&g_objpool, 
                                                  &msg->objpool_ctx, 
                                                  MTLK_TXMM_OBJ,
                                                  caller_slid))
#endif
  MTLK_INIT_FINALLY(txmm_msg, MTLK_OBJ_PTR(msg))
  MTLK_INIT_RETURN(txmm_msg, MTLK_OBJ_PTR(msg), mtlk_txmm_msg_cleanup, (msg))
}

void __MTLK_IFUNC
mtlk_txmm_msg_cleanup (mtlk_txmm_msg_t *msg)
{
  MTLK_ASSERT(msg != NULL);

  MTLK_CLEANUP_BEGIN(txmm_msg, MTLK_OBJ_PTR(msg))
#ifdef CPTCFG_IWLWAV_ENABLE_OBJPOOL
    MTLK_CLEANUP_STEP(txmm_msg, TXMM_MSG_OBJPOOL, MTLK_OBJ_PTR(msg), 
                      mtlk_objpool_remove_object, (&g_objpool, &msg->objpool_ctx, MTLK_TXMM_OBJ));
#endif
    MTLK_CLEANUP_STEP(txmm_msg, TXMM_MSG_DATA, MTLK_OBJ_PTR(msg), 
                      mtlk_txmm_msg_cancel, (msg));
    MTLK_CLEANUP_STEP(txmm_msg, TXMM_MSG_LOCK, MTLK_OBJ_PTR(msg), 
                      mtlk_osal_lock_cleanup,  (&msg->lock));
  MTLK_CLEANUP_END(txmm_msg, MTLK_OBJ_PTR(msg))
}

mtlk_txmm_data_t* __MTLK_IFUNC
mtlk_txmm_msg_get_empty_data (mtlk_txmm_msg_t *msg, mtlk_txmm_t *obj)
{
  mtlk_txmm_data_t*data = NULL;
  mtlk_txmm_t     *msg_obj;

  MTLK_ASSERT(msg != NULL);
  MTLK_ASSERT(obj != NULL);

  mtlk_osal_lock_acquire(&msg->lock);
  msg_obj = msg->obj;
  mtlk_osal_lock_release(&msg->lock);

  if (msg_obj == NULL) {
    data = _mtlk_txmm_attach_data(obj, msg);
  }
  else {
    MTLK_ASSERT(!"Message is already in use!");
  }

  return data;
}

void __MTLK_IFUNC
mtlk_txmm_msg_cancel (mtlk_txmm_msg_t *msg)
{
  mtlk_txmm_t *obj;

  MTLK_ASSERT(msg != NULL);

  mtlk_osal_lock_acquire(&msg->lock);
  obj = msg->obj;
  mtlk_osal_lock_release(&msg->lock);

  if (obj) {
    mtlk_osal_lock_acquire(&msg->lock);
    if (msg->data) {
      MTLK_ASSERT(msg->obj == obj);

      _mtlk_txmm_free(msg->obj->base, msg->data, msg);

      mtlk_osal_lock_acquire(&msg->lock);
      msg->data = NULL;
      msg->obj  = NULL;
    }
    mtlk_osal_lock_release(&msg->lock);
  }
}

int __MTLK_IFUNC
mtlk_txmm_msg_send (mtlk_txmm_msg_t  *msg, 
                    mtlk_txmm_clb_f   clb,
                    mtlk_handle_t     clb_usr_data,
                    uint32            timeout_ms)
{
  int res;

  MTLK_ASSERT(msg != NULL);
  MTLK_ASSERT(msg->data != NULL);
  MTLK_ASSERT(msg->obj != NULL);

  res = _mtlk_txmm_send_ex(msg->obj,
                           msg->data,
                           clb,
                           clb_usr_data,
                           timeout_ms);

  if (res != MTLK_ERR_OK) {
    mtlk_txmm_msg_cancel(msg);
  }

  return res;
}

int __MTLK_IFUNC
mtlk_txmm_msg_send_blocked (mtlk_txmm_msg_t  *msg, 
                            uint32            timeout_ms)
{
  MTLK_ASSERT(msg != NULL);
  MTLK_ASSERT(msg->data != NULL);
  MTLK_ASSERT(msg->obj != NULL);
  
  return _mtlk_txmm_send_blocked(msg->obj,
                                 msg->data,
                                 timeout_ms);
}

mtlk_txmm_data_t* __MTLK_IFUNC
mtlk_txmm_msg_init_with_empty_data (mtlk_txmm_msg_t *msg, 
                                    mtlk_txmm_t     *obj, 
                                    int             *err)
{
  mtlk_txmm_data_t* res = NULL;
  int               r   = mtlk_txmm_msg_init(msg);

  if (r == MTLK_ERR_OK) {
    res = mtlk_txmm_msg_get_empty_data(msg, obj);
    if (!res) {
      mtlk_txmm_msg_cleanup(msg);
      r = MTLK_ERR_NO_RESOURCES;
    }
  }

  if (err) {
    *err = r;
  }

  if (r == MTLK_ERR_OK)
    memset(res->payload, 0, obj->base->cfg.max_payload_size);

  return res;
}

void __MTLK_IFUNC
mtlk_txmm_base_get_stats (mtlk_txmm_base_t  *base,
                          mtlk_txmm_stats_t *stats)
{
  MTLK_ASSERT(base != NULL);
  MTLK_ASSERT(stats != NULL);

  mtlk_osal_lock_acquire(&base->lock);
  *stats = base->stats;
  mtlk_osal_lock_release(&base->lock);
}

void __MTLK_IFUNC
mtlk_txmm_get_stats (mtlk_txmm_t       *obj,
                     mtlk_txmm_stats_t *stats)
{
  MTLK_ASSERT(obj != NULL);
  MTLK_ASSERT(stats != NULL);

  mtlk_txmm_base_get_stats(obj->base, stats);
}

void __MTLK_IFUNC
mtlk_txmm_terminate_waiting_msg (mtlk_txmm_base_t *base)
{
  mtlk_osal_event_terminate(&base->bsend.evt);
}
