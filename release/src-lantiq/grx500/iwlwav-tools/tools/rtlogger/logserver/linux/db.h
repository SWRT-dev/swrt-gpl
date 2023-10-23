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
 *
 * Database
 *
 * Written by: Andrey Fidrya
 *
 */

#ifndef __DB_H__
#define __DB_H__

struct scd_entry
{
  int oid;
  int gid;
  int fid;
  int lid;
  char *text;

  struct scd_entry *next;
};

extern struct scd_entry *scd_data;

int db_init(void);
int db_destroy(void);
int db_register_scd_entry(int oid, int gid, int fid, int lid, char *text);

char *scd_get_text(int oid, int gid, int fid, int lid);

#endif // !__DB_H__

