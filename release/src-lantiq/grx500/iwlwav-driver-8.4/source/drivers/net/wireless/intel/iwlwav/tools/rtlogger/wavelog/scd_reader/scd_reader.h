#ifndef __SCD_READER_H__
#define __SCD_READER_H__

#define SCD_LINE_SIZE 1024

#define SCD_HASH_FLAGS_GID      0x01
#define SCD_HASH_FLAGS_TAG_OID  0x02
/* #define SCD_HASH_FLAGS_FID   0x04      Reserved */
#define SCD_HASH_FLAGS_LID      0x08
#define SCD_HASH_FLAGS_TMSG     0x10
/* #define SCD_HASH_FLAGS_SMSG  0x20      Reserved */
#define SCD_HASH_FLAGS_OBJ      0x40

#define SCD_EVT_TAG_SIG    'T'
#define SCD_EVT_TAG_HWSIG  'H'
#define SCD_EVT_TAG_MSG    'S'
#define SCD_EVT_TAG_GID    'G'
#define SCD_EVT_TAG_OID    'O'
#define SCD_EVT_TAG_OBJ    'D'

typedef struct _scd_evt_t {
  char tag;
  int oid;
  int gid;
  int fid;
  int lid;
  const char *name;
  void *user_data;
} scd_evt_t;

typedef struct _scd_info_t {
  guint32     signature;

  GHashTable  *lid_hash;
  GHashTable  *gid_hash;
  GHashTable  *tag_oid_hash;
  GHashTable  *obj_hash;
  GHashTable  *tmsg_hash;

  scd_evt_t   *scd_evt_table;
  guint       scd_evt_table_size;
  guint       scd_proc_closed_flag;

} scd_info_t;


extern scd_info_t*
scd_init(guint32 hash_flags);

extern gint
process_scd(const char *scd_fn, scd_info_t *scd_info);

extern void
scd_process_finish(scd_info_t *scd_info);

extern const char*
scd_get_gid_name(scd_info_t *scd_info, guint32 oid, guint32 gid);

extern const char*
scd_get_oid_name(scd_info_t *scd_info, guint32 oid);

extern const char*
scd_get_oid_elfname(scd_info_t *scd_info, guint32 oid);

extern void*
scd_get_data_by_ids(scd_info_t *scd_info, guint32 oid, guint32 gid, guint32 fid, guint32 lid);

extern const char*
scd_get_msg_by_ids(scd_info_t *scd_info, guint32 oid, guint32 gid, guint32 fid, guint32 lid);

extern scd_evt_t*
scd_get_by_tmsg(scd_info_t *scd_info, const gchar *msg);

scd_evt_t*
scd_get_by_ids(scd_info_t *scd_info, guint32 oid, guint32 gid, guint32 fid, guint32 lid);


#endif /* __SCD_READER_H__ */
