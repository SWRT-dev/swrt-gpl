/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#ifndef __MTLK_DF_DEFS_H__
#define __MTLK_DF_DEFS_H__

typedef struct _mtlk_user_request_t mtlk_user_request_t; /*!< User request object provided to core */

struct _mtlk_hw_vft_t;
typedef struct _mtlk_vap_manager_t mtlk_vap_manager_t;
typedef struct _mtlk_hw_api_t      mtlk_hw_api_t;
typedef struct _mtlk_vap_handle_t *mtlk_vap_handle_t;

#define _mtlk_core_t nic
typedef struct _mtlk_core_t     mtlk_core_t;

struct mtlk_hw_t;
typedef struct _mtlk_hw_t       mtlk_hw_t;

struct _mtlk_irq_handler_data_t
{
  mtlk_hw_t             *hw;
  struct tasklet_struct *irq_tasklets; /* for easy access to tasklet array */
  uint32                 tasklet;     /* index in array of irq_tasklets */
  uint32                 irq_no;
  uint32                 status;
};

typedef struct _mtlk_irq_handler_data_t mtlk_irq_handler_data;

typedef struct _mtlk_df_t       mtlk_df_t;
typedef struct _mtlk_pdb_t      mtlk_pdb_t;

typedef struct _sta_entry       sta_entry;

typedef struct net_device       mtlk_ndev_t;
typedef struct sk_buff          mtlk_nbuf_t;
typedef struct sk_buff_head     mtlk_nbuf_list_t;
typedef struct sk_buff          mtlk_nbuf_list_entry_t;

#endif /* __MTLK_DF_DEFS_H__ */

