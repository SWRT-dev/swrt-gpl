#ifndef _SCD_DWARF_PARSER_H_
#define _SCD_DWARF_PARSER_H_

#define DIES_MEMB_FLAG_IS_LAST   1
#define DIES_MEMB_FLAG_IS_PARENT 2
#define DIES_MEMB_FLAG_IS_ROOT   4

typedef enum _dies_memb_type_t{
  DIES_MEMB_TYPE_STRUCT,
  DIES_MEMB_TYPE_ARRAY,
  DIES_MEMB_TYPE_FIELD,
  DIES_MEMB_TYPE_LAST
} dies_memb_type_t;

typedef struct _dies_memb_t {

  dies_memb_type_t  memb_type;
  char              *type_name;
  char              *field_name;
  guint32           offset; /* Relative to parent */
  guint32           size;
  guint32           bit_offset;
  guint32           bit_size;
  ftenum_t          format;
  base_display_e    display;
  value_string      *strings;

  gint self_id;
  gint flag;

  int   hf_id;              /* field ID filled by WS on registration */
  gint  ett_id;             /* subtree ID filled by WS on registration */

} dies_memb_t;

typedef struct _dies_pool_t {
  char        *elf_fname;
  dies_memb_t *dies;
  guint        size;
} dies_pool_t;

#define RESULT_OK   0
#define RESULT_FAIL 1

int dwarf2_init(void);
int scd_dwarf_parse(const char *scd_fn, GSList **dies_pools_list);
int dwarf2_finish(void);

#endif /* _SCD_DWARF_PARSER_H_ */
