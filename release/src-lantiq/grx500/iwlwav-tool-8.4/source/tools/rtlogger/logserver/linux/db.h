/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
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

