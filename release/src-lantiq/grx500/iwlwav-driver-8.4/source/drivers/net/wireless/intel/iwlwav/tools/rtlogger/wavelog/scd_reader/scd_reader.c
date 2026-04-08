/* TODO: add SCD_close to free resources */
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <glib.h>

#include "scd_reader.h"

#define RESULT_OK   0
#define RESULT_FAIL 1

#define SCD_ALLOC_STEP_SIZE 2048
#define SCD_TYPENAME_SIZE (SCD_LINE_SIZE-16)

#define SCD_INFO_SIGNATURE 0x8343BEAD   /* SC */

gboolean
scd_evt_ids_is_equal(gconstpointer scd_evt1_v, gconstpointer scd_evt2_v){

  const scd_evt_t *scd_evt1 = (scd_evt_t *)scd_evt1_v;
  const scd_evt_t *scd_evt2 = (scd_evt_t *)scd_evt2_v;

  return ((scd_evt1->lid == scd_evt2->lid) &&
          (scd_evt1->fid == scd_evt2->fid) &&
          (scd_evt1->gid == scd_evt2->gid) &&
          (scd_evt1->oid == scd_evt2->oid));
}

gboolean
scd_evt_tag_oid_is_equal(gconstpointer scd_evt1_v, gconstpointer scd_evt2_v){

  const scd_evt_t *scd_evt1 = (scd_evt_t *)scd_evt1_v;
  const scd_evt_t *scd_evt2 = (scd_evt_t *)scd_evt2_v;

  return ((scd_evt1->tag == scd_evt2->tag) &&
          (scd_evt1->oid == scd_evt2->oid));
}

guint
scd_evt_hash_tag_oid_key(gconstpointer scd_evt_v){

  const scd_evt_t *scd_evt = (scd_evt_t *)scd_evt_v;
  //  0   tag   0    0
  //  0    0    0   gid

  return (scd_evt->tag << 16) +
         (scd_evt->oid <<  0);
}

guint
scd_evt_hash_ids_key(gconstpointer scd_evt_v){

  const scd_evt_t *scd_evt = (scd_evt_t *)scd_evt_v;
  // oid   0    0    0
  // gid   0    0    0
  // fid  fid   0    0
  //  0    0   lid  lid

  return (scd_evt->oid << 24) +
         (scd_evt->gid << 24) +
         (scd_evt->fid << 16) +
         (scd_evt->lid <<  0);
}

gboolean
scd_evt_msg_is_equal(gconstpointer scd_evt1_v, gconstpointer scd_evt2_v){

  const scd_evt_t *scd_evt1 = (scd_evt_t *)scd_evt1_v;
  const scd_evt_t *scd_evt2 = (scd_evt_t *)scd_evt2_v;

  return (strcmp(scd_evt1->name, scd_evt2->name) == 0);
}

guint
scd_evt_hash_msg_key(gconstpointer scd_evt_v){

  const scd_evt_t *scd_evt = (scd_evt_t *)scd_evt_v;

  return g_str_hash((gconstpointer)scd_evt->name);
}

scd_info_t*
scd_init(guint32 hash_flags){

  scd_info_t  *scd_info;

  scd_info = (scd_info_t*)g_malloc(sizeof(*scd_info));
  if (!scd_info)
    return NULL;

  scd_info->signature = SCD_INFO_SIGNATURE;
  scd_info->gid_hash = NULL;
  scd_info->tag_oid_hash = NULL;
  scd_info->obj_hash = NULL;
//  scd_info->fid_hash = NULL;
  scd_info->lid_hash = NULL;
  scd_info->tmsg_hash = NULL;

  scd_info->scd_evt_table = NULL;
  scd_info->scd_evt_table_size = 0;

  if (hash_flags & SCD_HASH_FLAGS_GID){
    scd_info->gid_hash = g_hash_table_new(scd_evt_hash_ids_key, scd_evt_ids_is_equal);
  }

  if (hash_flags & SCD_HASH_FLAGS_TAG_OID){
    scd_info->tag_oid_hash = g_hash_table_new(scd_evt_hash_tag_oid_key, scd_evt_tag_oid_is_equal);
  }

  if (hash_flags & SCD_HASH_FLAGS_OBJ){
    scd_info->obj_hash = g_hash_table_new(scd_evt_hash_msg_key, scd_evt_msg_is_equal);
  }

  if (hash_flags & SCD_HASH_FLAGS_LID){
    scd_info->lid_hash = g_hash_table_new(scd_evt_hash_ids_key, scd_evt_ids_is_equal);
  }

  if (hash_flags & SCD_HASH_FLAGS_TMSG){
    scd_info->tmsg_hash = g_hash_table_new(scd_evt_hash_msg_key, scd_evt_msg_is_equal);
  }

  scd_info->scd_proc_closed_flag = 0;

  return scd_info;
}

static gint
parse_scd(const char *scd_fn, scd_evt_t **scd_evt_table, guint *scd_evt_table_size){

  gint res = RESULT_OK;
  FILE *fp;
  char line[SCD_LINE_SIZE];

  scd_evt_t *tbl;
  guint curr_size;
  size_t i;

  fp = fopen(scd_fn, "r");
  if (fp == NULL)
    return RESULT_FAIL;

  tbl = *scd_evt_table;
  curr_size = *scd_evt_table_size;

  i = curr_size;

  while (fgets(line, SCD_LINE_SIZE, fp) != NULL) {
    gchar buf[SCD_TYPENAME_SIZE];
    gchar *s = &line[2];
    gchar tag;
    scd_evt_t *scd_entry;

    if (curr_size == i) { /* size limit is reached or not allocated yet */
      curr_size += SCD_ALLOC_STEP_SIZE;
      tbl = (scd_evt_t*)g_realloc(tbl, curr_size * sizeof(scd_evt_t));
      if (NULL == tbl){
        res = RESULT_FAIL;
        goto FINISH;
      }else{
        *scd_evt_table_size = i;
        *scd_evt_table = tbl;
      }
    }

    tag = line[0];
    scd_entry = &(tbl)[i++];

    switch (tag){
      case SCD_EVT_TAG_GID:

          sscanf(s, "%d %d %[^\n]s", &scd_entry->oid, &scd_entry->gid, buf);
          scd_entry->name = g_strdup(buf);
          scd_entry->user_data = NULL;
          scd_entry->fid = 0;
          scd_entry->lid = 0;
          scd_entry->tag = tag;
          break;

      case SCD_EVT_TAG_OID:
      case SCD_EVT_TAG_OBJ:
          sscanf(s, "%d %[^\n]s", &scd_entry->oid, buf);
          scd_entry->name = g_strdup(buf);
          scd_entry->user_data = NULL;
          scd_entry->gid = 0;
          scd_entry->fid = 0;
          scd_entry->lid = 0;
          scd_entry->tag = tag;
          break;

      case SCD_EVT_TAG_SIG:
      case SCD_EVT_TAG_HWSIG:
      case SCD_EVT_TAG_MSG:

          /* Process STRUCTURE and TYPEDEFS */
          sscanf(s, "%d %d %d %d %[^\n]s", &scd_entry->oid, &scd_entry->gid, &scd_entry->fid, &scd_entry->lid, buf);
          scd_entry->name = g_strdup(buf);
          scd_entry->user_data = NULL;
          scd_entry->tag = tag;
          scd_entry->user_data = NULL;
          break;

      default:
          break;
    }

  }

FINISH:

  if (fp){
    fclose(fp);
  }

  if (res == RESULT_OK) {
    *scd_evt_table_size = i;
  }

  return res;
}

gint
process_scd(const char *scd_fn, scd_info_t *scd_info){

  if (scd_info->scd_proc_closed_flag)
    return RESULT_FAIL;       /* SCD table already hashed and can't be reallocated
                                 Rework realloc() to overcome this limitation      */

  return parse_scd(scd_fn, &scd_info->scd_evt_table, &scd_info->scd_evt_table_size);
}

static void
fillup_scd_hashes(scd_info_t *scd_info){

  guint i;
  GHashTable* scd_ids_hash;
  GHashTable* scd_str_hash;

  guint       scd_evt_tbl_size;
  scd_evt_t   *scd_evt;

  scd_evt = scd_info->scd_evt_table;
  scd_evt_tbl_size = scd_info->scd_evt_table_size;

  for (i = 0; i < scd_evt_tbl_size; i++){

    scd_str_hash = NULL;
    scd_ids_hash = NULL;

    /* Choose proper HASH for SCD entry by its TAG */
    switch(scd_evt->tag){
      case SCD_EVT_TAG_SIG:
      case SCD_EVT_TAG_HWSIG:
        scd_ids_hash = scd_info->lid_hash;
        scd_str_hash = scd_info->tmsg_hash;
        break;
      case SCD_EVT_TAG_MSG:
        scd_ids_hash = scd_info->lid_hash;
        break;
      case SCD_EVT_TAG_GID:
        scd_ids_hash = scd_info->gid_hash;
        break;
      case SCD_EVT_TAG_OID:
        scd_ids_hash = scd_info->tag_oid_hash;
        break;
      case SCD_EVT_TAG_OBJ:
        scd_str_hash = scd_info->obj_hash;
        scd_ids_hash = scd_info->tag_oid_hash;
        break;
      default:
        break;
    }

    if (scd_ids_hash){
      g_hash_table_insert(scd_ids_hash, (gpointer)scd_evt, (gpointer)scd_evt);
    }

    if (scd_str_hash){
      g_hash_table_insert(scd_str_hash, (gpointer)scd_evt, (gpointer)scd_evt);
    }

    scd_evt++;
  }

  return;
}

void
scd_process_finish(scd_info_t *scd_info){

  /* Note: The function required to support several SCD files
     Hash can't be filled on-fly because data structures might be
     reallocated
  */
  fillup_scd_hashes(scd_info);
  scd_info->scd_proc_closed_flag = 1;

  return;
}


const char*
scd_get_oid_name(scd_info_t *scd_info, guint32 oid){

  scd_evt_t  scd_evt_key;
  scd_evt_t  *scd_evt;

  scd_evt_key.oid = oid;
  scd_evt_key.tag = SCD_EVT_TAG_OID;

  scd_evt = (scd_evt_t*)g_hash_table_lookup(scd_info->tag_oid_hash, &scd_evt_key);

  if (scd_evt) return scd_evt->name;
  else return NULL;
}

const char*
scd_get_oid_elfname(scd_info_t *scd_info, guint32 oid){

  scd_evt_t  scd_evt_key;
  scd_evt_t  *scd_evt;

  scd_evt_key.oid = oid;
  scd_evt_key.tag = SCD_EVT_TAG_OBJ;

  scd_evt = (scd_evt_t*)g_hash_table_lookup(scd_info->tag_oid_hash, &scd_evt_key);

  if (scd_evt) return scd_evt->name;
  else return NULL;
}

const char*
scd_get_gid_name(scd_info_t *scd_info, guint32 oid, guint32 gid){

  scd_evt_t  scd_evt_key;
  scd_evt_t  *scd_evt;

  scd_evt_key.oid = oid;
  scd_evt_key.gid = gid;
  scd_evt_key.fid = 0;
  scd_evt_key.lid = 0;

  scd_evt = (scd_evt_t*)g_hash_table_lookup(scd_info->gid_hash, &scd_evt_key);

  if (scd_evt) return scd_evt->name;
  else return NULL;
}

scd_evt_t*
scd_get_by_ids(scd_info_t *scd_info, guint32 oid, guint32 gid, guint32 fid, guint32 lid){

  scd_evt_t  scd_evt_key;
  scd_evt_t  *scd_evt;

  scd_evt_key.oid = oid;
  scd_evt_key.gid = gid;
  scd_evt_key.fid = fid;
  scd_evt_key.lid = lid;

  scd_evt = (scd_evt_t*)g_hash_table_lookup(scd_info->lid_hash, &scd_evt_key);

  return scd_evt;
}


const char*
scd_get_msg_by_ids(scd_info_t *scd_info, guint32 oid, guint32 gid, guint32 fid, guint32 lid){

  scd_evt_t  *scd_evt;

  scd_evt = scd_get_by_ids(scd_info, oid, gid, fid, lid);

  if (scd_evt) return scd_evt->name;
  else return NULL;
}

void*
scd_get_data_by_ids(scd_info_t *scd_info, guint32 oid, guint32 gid, guint32 fid, guint32 lid){

  scd_evt_t  *scd_evt;

  scd_evt = scd_get_by_ids(scd_info, oid, gid, fid, lid);

  if (scd_evt) return scd_evt->user_data;
  else return NULL;
}

scd_evt_t*
scd_get_by_tmsg(scd_info_t *scd_info, const gchar *msg){

  scd_evt_t  scd_evt_key;

  scd_evt_key.name = msg;

  return (scd_evt_t*)g_hash_table_lookup(scd_info->tmsg_hash, &scd_evt_key);

}
