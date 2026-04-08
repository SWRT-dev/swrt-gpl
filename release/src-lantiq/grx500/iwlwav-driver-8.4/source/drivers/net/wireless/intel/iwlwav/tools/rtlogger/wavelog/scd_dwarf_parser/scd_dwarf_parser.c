#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

#include <dwarf.h>
#include <libdwarf.h>

#include <glib.h>
#include <scd_reader.h>

#include "scd_dwarf_wenv.h"
#include "scd_dwarf_parser.h"

#define DEBUG
#undef  DBG_FUNC_TRACE

#ifdef DEBUG
#define DBG_PRINT(_format_, ...) \
        fprintf(stderr, "%s:%d:\t" _format_, __FUNCTION__, __LINE__, ## __VA_ARGS__)
#else
#define DBG_PRINT(...)
#endif

#ifdef  DBG_FUNC_TRACE
#define DBG_FUNC_IN             DBG_PRINT(">>> %s\n", __FUNCTION__)
#define DBG_FUNC_OUT            DBG_PRINT("<<< %s\n", __FUNCTION__)
#else
#define DBG_FUNC_IN
#define DBG_FUNC_OUT
#endif

#define FORMAT_TYPE(s,f) { char *p; p = strdup(s); sprintf(s,f,p); free(p); }
#define FORMAT_TYPE2(s,t,f) { char *p; p = strdup(s); sprintf(s,f,t,p); free(p); }
#define FORMAT_TYPE2_REV(s,t,f) { char *p; p = strdup(s); sprintf(s,f,p,t); free(p); }

#define DIE_SIZE_INIT (1024*4)
#define DIE_SIZE_STEP (1024*4)

#define UNUSED_PARAM(_param) ((_param)=(_param))

int is_info = 1;

static scd_info_t *scd_info;


static int process_elf(dies_pool_t *dies_pool);

static void process_dwarf(void);
static void process_siblings(Dwarf_Die die);
static void maybe_process_die(Dwarf_Die die);

static dies_memb_t *process_record(Dwarf_Die die, Dwarf_Half parent_tag);
static dies_memb_t *process_children(Dwarf_Die die);
static int die_nof_children(Dwarf_Die die);
static dies_memb_t *add_record(Dwarf_Die die);
static Dwarf_Die typedef_to_type(Dwarf_Die die);
static void record_format(dies_memb_t *rec, Dwarf_Die die);
static dies_memb_t *cur_record(void);
static dies_memb_t *prev_record(dies_memb_t *rec);
static unsigned process_subrange(Dwarf_Die die);

static void dies_check_memory(void);

static int type_is_processed(char *name);

static char* die_get_name(Dwarf_Die die);
static char* die_get_type_name(Dwarf_Die die);
static void die_get_type_name_(Dwarf_Die die, char *res);
static Dwarf_Half die_get_tag(Dwarf_Die die);
static unsigned die_attr_size(Dwarf_Die die);
static Dwarf_Die die_get_type(Dwarf_Die die);
static unsigned die_get_size(Dwarf_Die die);
static unsigned die_attr_offset(Dwarf_Die die);
static unsigned die_attr_bit_offset(Dwarf_Die die);
static unsigned die_attr_bit_size(Dwarf_Die die);
static int die_attr_const_value(Dwarf_Die die);

static Dwarf_Debug   s_dbg;
static dies_memb_t  *s_dies;
static unsigned      s_dies_size;
static unsigned      s_dies_index = 0;
static dies_memb_t  *s_last_rec;
static int           s_enum_index = -1;

int dwarf2_init(void)
{
  int res = RESULT_OK;
  return res;
}

int scd_dwarf_parse(const char *scd_fn, GSList **dies_pools_list)
{

  int   res;

  GHashTableIter iter;

  scd_evt_t  *scd_evt_key;
  scd_evt_t  *scd_evt;

  res = RESULT_OK;

  scd_info = scd_init(SCD_HASH_FLAGS_TMSG | SCD_HASH_FLAGS_OBJ);
  if (scd_info == NULL)
    return RESULT_FAIL;

  res = process_scd(scd_fn, scd_info);

  scd_process_finish(scd_info);

  // Fill up DIES pools list by  processing all OBJ files specified in SCD.
  *dies_pools_list = NULL;
  g_hash_table_iter_init(&iter, scd_info->obj_hash);
  while(g_hash_table_iter_next(&iter, (gpointer*)&scd_evt_key, (gpointer*)&scd_evt)){
    dies_pool_t *dies_pool;

    // Add new entry to dies pool list
    dies_pool = (dies_pool_t*)malloc(sizeof(dies_pool_t));
    dies_pool->elf_fname = g_strdup(scd_evt->name);
    dies_pool->dies = NULL;
    dies_pool->size = 0;

    *dies_pools_list = g_slist_append(*dies_pools_list, (gpointer)dies_pool);

    // Fillup pool with entries from particular ELF file specified in SCD
    // A DIE entry created for SCD entries where TAG = SCD_EVT_TAG_SIG
    res = process_elf(dies_pool);
  }

  /*return RESULT_OK;*/
  return res;
}

int dwarf2_finish(void)
{
  unsigned i;
  int res = RESULT_OK;

  /* TODO: call scd_finish() */

  for(i = 0; i < s_dies_size; ++i) {
    dies_memb_t *p = &s_dies[i];
    if (p->type_name != NULL) free(p->type_name);
    if (p->field_name != NULL) free(p->field_name);
    if (p->strings != NULL) free(p->strings);
  }
  free(s_dies);

  return res;
}

static int type_is_processed(char *name)
{
  unsigned i;
  for (i = 0; i < s_dies_index; ++i) {
    dies_memb_t *pdie = &s_dies[i];
    if ((pdie->flag & DIES_MEMB_FLAG_IS_ROOT) && (0 == strcmp(name, pdie->type_name))){
      return 1;
    }
  }
  return 0;
}

static int process_elf(dies_pool_t *dies_pool)
{
  int res = RESULT_OK;

  int fd = -1;
  FILE *fp;
  Dwarf_Debug dbg = 0;
  Dwarf_Error error;
  Dwarf_Handler err_handler;
  Dwarf_Ptr err_arg;

  DBG_PRINT(">>> process_elf('%s')\n", dies_pool->elf_fname);

  fp = fopen(dies_pool->elf_fname, "rb");

  if (fp == NULL) {
    fprintf(stderr, "opening '%s' failed: %s\n", dies_pool->elf_fname, strerror(errno));
    res = RESULT_FAIL;
    goto FINISH;
  }
  fd = fileno(fp);

  err_arg = NULL;
  err_handler = NULL;
  res = dwarf_init(fd, DW_DLC_READ, err_handler, err_arg, &dbg, &error);

  if (res != DW_DLV_OK) {
    fprintf(stderr, "dwarf_init failed\n");
    res = RESULT_FAIL;
    goto FINISH;
  }

  dies_pool->size = DIE_SIZE_INIT;
  dies_pool->dies = (dies_memb_t*)malloc(dies_pool->size * sizeof(dies_memb_t));
  if (NULL == dies_pool->dies) {
    res = RESULT_FAIL;
    goto FINISH;
  }

  s_dbg        = dbg;
  s_dies       = dies_pool->dies;
  s_dies_size  = dies_pool->size;
  s_dies_index = 0;

  process_dwarf();

  dies_pool->dies = s_dies;
  dies_pool->size = s_dies_index;

  if (s_last_rec) {
    s_last_rec->flag |= DIES_MEMB_FLAG_IS_LAST;
  }

  res = dwarf_finish(dbg, &error);

  if (res != DW_DLV_OK) {
    fprintf(stderr, "dwarf_finish failed\n");
    return EXIT_FAILURE;
  }
FINISH:
  DBG_PRINT("<<< process_elf\n");
  return res;
}

void process_dwarf(void)
{
  Dwarf_Unsigned cu_header = 0;
  Dwarf_Error error;

  while (1) {
    Dwarf_Die no_die = 0;
    Dwarf_Die cu_die = 0;
    int res;
    res = dwarf_next_cu_header_c(s_dbg, is_info, 0, 0, 0, 0, 0, 0, 0, 0, &cu_header, &error);

    if (res == DW_DLV_ERROR) {
      fprintf(stderr, "dwarf_next_cu_header failed\n");
      exit(EXIT_FAILURE);
    }
    if (res == DW_DLV_NO_ENTRY) {
      return; /* all done */
    }
    res = dwarf_siblingof(s_dbg, no_die, &cu_die, &error);
    if (res == DW_DLV_ERROR) {
      fprintf(stderr, "dwarf_sublingof failed\n");
      exit(EXIT_FAILURE);
    }
    if (res == DW_DLV_NO_ENTRY) {
      fprintf(stderr, "dwarf_sublingof: no DIE entry in CU\n");
      exit(EXIT_FAILURE);
    }
    process_siblings(cu_die);
    dwarf_dealloc(s_dbg, cu_die, DW_DLA_DIE);
  }
}

static void process_siblings(Dwarf_Die die)
{
  int res;
  Dwarf_Die cur_die = die;
  Dwarf_Die child = 0;
  Dwarf_Error error;

  DBG_FUNC_IN;

  maybe_process_die(die);

  while (1) {
    Dwarf_Die sib_die = 0;
    res = dwarf_child(cur_die, &child, &error);
    if (res == DW_DLV_ERROR) {
      fprintf(stderr, "dwarf_child failed\n");
      exit(EXIT_FAILURE);
    }
    if (res == DW_DLV_OK) {
      process_siblings(child);
    }
    res = dwarf_siblingof(s_dbg, cur_die, &sib_die, &error);
    if (res == DW_DLV_ERROR) {
      fprintf(stderr, "dwarf_sublingof failed\n");
      exit(EXIT_FAILURE);
    }
    if (res == DW_DLV_NO_ENTRY) {
      break; /* all done at current level */
    }
    if (cur_die != die) {
      dwarf_dealloc(s_dbg, cur_die, DW_DLA_DIE);
    }
    cur_die = sib_die;
    maybe_process_die(cur_die);
  }

  DBG_FUNC_OUT;
}

static void maybe_process_die(Dwarf_Die die)
{
  char *name = NULL;
  dies_memb_t *root_memb;
  Dwarf_Half   tag;

  name = die_get_name(die);
  tag = die_get_tag(die);

  if (name
      && (DW_TAG_typedef == tag || DW_TAG_structure_type == tag || DW_TAG_base_type == tag)
      && scd_get_by_tmsg(scd_info, name)
      && die_get_size(die) > 0
      && !type_is_processed(name)) {
    DBG_PRINT("PROCESS RECORD(%s)\n", name?name:"");
    root_memb = process_record(die, 0);
    root_memb->flag |= DIES_MEMB_FLAG_IS_ROOT;
  } else {
    /* DBG_PRINT("SKIP RECORD(%s), tag 0x%02x\n", name?name:"", (int)tag); */
  }

  if (name) dwarf_dealloc(s_dbg, name, DW_DLA_STRING);
}

#define REC (&s_dies[index])

static dies_memb_t *process_record(Dwarf_Die die, Dwarf_Half parent_tag)
{
  dies_memb_t *rec = 0;
  unsigned   index = 0;
  Dwarf_Half   tag;

  DBG_FUNC_IN;

  tag = die_get_tag(die);

  DBG_PRINT("process_record(0x%02x,0x%02x)\n", tag, parent_tag);

  switch (tag) {
  case DW_TAG_base_type:        /* should be processed only for ROOT entry */
  case DW_TAG_volatile_type:
  case DW_TAG_typedef:
  {
    Dwarf_Die type_die = die_get_type(die);

    DBG_PRINT("TYPEDEF: \n");

    switch (parent_tag) {
    case 0:
    {
      char *name;

      rec   = add_record(die);
      index = s_dies_index-1;

      name  = die_get_name(die);

      REC->memb_type  = DIES_MEMB_TYPE_STRUCT;
      REC->type_name  = strdup(name);
      REC->field_name = strdup("");
      REC->offset     = die_attr_offset(die);
      REC->size       = die_get_size(die);

      if (tag != DW_TAG_base_type) {
        process_record(type_die, tag);
      }

      if (name) dwarf_dealloc(s_dbg, name, DW_DLA_STRING);
      s_last_rec = REC;
      break;
    }
    default:
    {
      if (tag != DW_TAG_base_type) {
        process_record(type_die, tag);
      }
      break;
    }
    }
    break;
  }
  case DW_TAG_structure_type:
  case DW_TAG_union_type:
  {
    dies_memb_t *last = NULL;

    DBG_PRINT("TYPE: \n");
    switch (parent_tag) {
    case 0:
    {
      char *name;

      rec   = add_record(die);
      index = s_dies_index-1;

      name = die_get_name(die);

      REC->memb_type  = DIES_MEMB_TYPE_STRUCT;
      REC->type_name  = strdup(name);
      REC->field_name = strdup("");
      REC->offset     = die_attr_offset(die);
      REC->size       = die_get_size(die);

      if (die_nof_children(die)) {
        REC->flag |= DIES_MEMB_FLAG_IS_PARENT;
        DBG_PRINT("LINE %d: set flag IS_PARENT\n", __LINE__);
      }

      last = process_children(die);

      if (name) dwarf_dealloc(s_dbg, name, DW_DLA_STRING);
      s_last_rec = REC;
      break;
    }
    default:
    {
      dies_memb_t *cur = cur_record();

      cur->memb_type = DIES_MEMB_TYPE_STRUCT;

      if (0 == strlen(cur->type_name)) {
        if (DW_TAG_structure_type == tag) {
          free(cur->type_name);
          cur->type_name = strdup("struct");
        }
        else if (DW_TAG_union_type == tag) {
          free(cur->type_name);
          cur->type_name = strdup("union");
        }

        if (0 == strlen(cur->field_name) &&
           (DW_TAG_structure_type == tag || DW_TAG_union_type == tag)) {
          free(cur->field_name);
          cur->field_name = strdup("UNNAMED");
        }
      }

      if (die_nof_children(die)) {
        cur->flag |= DIES_MEMB_FLAG_IS_PARENT;
        DBG_PRINT("LINE %d: set flag IS_PARENT\n", __LINE__);
      }

      last = process_children(die);
    }
    }
    if (last)
      last->flag |= DIES_MEMB_FLAG_IS_LAST;
    break;
  }
  case DW_TAG_member:
  {
    char *name;
    char *type_name;
    Dwarf_Die type_die;
    Dwarf_Half type_tag;

    rec       = add_record(die);
    index     = s_dies_index-1;

    name      = die_get_name(die);
    type_die  = die_get_type(die);
    type_name = die_get_type_name(type_die);
    type_tag  = die_get_tag(type_die);

    REC->memb_type  = DIES_MEMB_TYPE_FIELD;
    REC->type_name  = strdup(type_name);
    REC->field_name = strdup(name?name:"");
    REC->offset     = die_attr_offset(die);
    REC->size       = die_get_size(die);
    REC->bit_size   = die_attr_bit_size(die);
    REC->bit_offset = die_attr_bit_offset(die);

/* We need to recalculate bit_offset */
/* TODO: should be made according to ELF, i.e. per OID */
#if defined (__BIG_ENDIAN__)
#else /* __LITTLE_ENDIAN__ */
    REC->bit_offset = (8 * REC->size) - REC->bit_size - REC->bit_offset;
    /* ready */
#endif

    record_format(REC, type_die);

    DBG_PRINT("MEMBER: name='%s' type_name='%s' type_tag='0x%02x' size='%d' offset='%d'\n", name, type_name, type_tag, REC->size, REC->offset);

    process_record(type_die, DW_TAG_member);

    if (name) dwarf_dealloc(s_dbg, name, DW_DLA_STRING);
    if (type_name) free(type_name);
    if (type_die) dwarf_dealloc(s_dbg, type_die, DW_DLA_DIE);
    break;
  }
  case DW_TAG_enumeration_type:
  {
    int nof_children = die_nof_children(die);
    dies_memb_t *cur = cur_record();

    DBG_PRINT("ENUMERATION\n");

    cur->strings = (value_string*)malloc((nof_children + 1) * sizeof(value_string));
    s_enum_index = 0;

    process_children(die);

    cur->strings[s_enum_index].value  = 0;
    cur->strings[s_enum_index].string = NULL;

    break;
  }
  case DW_TAG_enumerator:
  {
    dies_memb_t *cur   = cur_record();
    int          value = die_attr_const_value(die);
    char        *name  = die_get_name(die);

    DBG_PRINT("ENUMERATOR: %d -> %s(%d)\n", s_enum_index, name, value);

    cur->strings[s_enum_index].value  = value;
    cur->strings[s_enum_index].string = strdup(name);

    s_enum_index += 1;

    if (name) dwarf_dealloc(s_dbg, name, DW_DLA_STRING);

    break;
  }
  case DW_TAG_array_type:
  {
    Dwarf_Die    type_die   = NULL;
    dies_memb_t *cur        = NULL;
    dies_memb_t *last       = NULL;
    char        *type_name  = NULL;
    char        *field_name = NULL;
    int          type_size  = 0;
    int          array_size = 0;
    int          type_flag;

    array_size = die_attr_size(die);
    DBG_PRINT("ARRAY: array_size: %d\n", array_size);

    switch (parent_tag) {
    case DW_TAG_member:
    case DW_TAG_array_type:
    case DW_TAG_typedef:
    case DW_TAG_volatile_type:
    {
      cur = cur_record();
      cur->memb_type  = DIES_MEMB_TYPE_ARRAY;
      field_name = cur->field_name;
      type_die  = die_get_type(die);
      type_name = die_get_type_name(type_die);
      type_size = die_get_size(type_die);

      if (die_nof_children(die)) {
        cur->flag |= DIES_MEMB_FLAG_IS_PARENT;
        DBG_PRINT("LINE %d: set flag IS_PARENT\n", __LINE__);
      }

      last = process_children(die);
      if (last && type_die) {
        dies_memb_t *prev = NULL;
        unsigned int range = last->size;
        char range_s[128];
        last->type_name  = strdup(type_name);
        last->field_name = strdup(field_name);
        last->size       = type_size;
        last->flag      |= DIES_MEMB_FLAG_IS_LAST;

        record_format(last, type_die);

        process_record(type_die, DW_TAG_array_type);

        DBG_PRINT(" cur->size: %u, type_name '%s', type_size %d, range %d, \n",
            (int)cur->size, cur->type_name, type_size, range);

        prev = prev_record(cur);

        type_flag = 1; /* TRUE -> change type_name */

        while (DIES_MEMB_TYPE_ARRAY == prev->memb_type &&
                DW_TAG_array_type == parent_tag) {

          DBG_PRINT("prev->size: %u, type_name '%s'\n", (int)prev->size, prev->type_name);

          prev->size *= range;

          if (type_flag) {
            /* append to the prev->type_name the array dimensions string's part from the current
               example:  cur->type_name = "u16[4][3]"
                        prev->type_name = "u16[2]"
                result: prev->type_name = "u16[2][4][3]"
             */
            char *cp1 = strchr(cur->type_name, '[');
            if (NULL == cp1) {
                break;
            }
            sprintf(range_s, "%s%s", prev->type_name, cp1);
            free(prev->type_name);
            prev->type_name = strdup(range_s);

            type_flag = 0; /* FALSE -> don't do this again */
          }

          DBG_PRINT("prev->size: %u, type_name '%s'\n", (int)prev->size, prev->type_name);

          cur = prev;
          prev = prev_record(cur);
        }
      }
      if (type_name) free(type_name);
      if (type_die) dwarf_dealloc(s_dbg, type_die, DW_DLA_DIE);
      break;
    }
    default:
      ; /* do nothing */
    }
    break;
  }
  case DW_TAG_subrange_type:
  {
    int range;

    DBG_PRINT("SUBRANGE:\n");

    rec   = add_record(die);
    index = s_dies_index-1;

    REC->memb_type  = DIES_MEMB_TYPE_FIELD;

    range = process_subrange(die);
    REC->size = range;

    DBG_PRINT("SUBRANGE: %u\n", range);

    break;
  }
  default:
    ;
  }

  DBG_FUNC_OUT;

  fflush(stderr);

  return rec?REC:0;
}

static dies_memb_t *process_children(Dwarf_Die die)
{
  Dwarf_Die child_die = NULL;
  Dwarf_Die sib_die   = NULL;
  Dwarf_Die cur_die   = NULL;
  dies_memb_t *last_rec = NULL;
  Dwarf_Error error;
  int res;

  Dwarf_Half   tag1, tag2;
  dies_memb_t  *curr, *prev;
  int           siz1, siz2;

  char range_s[128];

  DBG_PRINT(">>>>> process_children\n");

  res = dwarf_child(die, &child_die, &error);
  if (res == DW_DLV_ERROR) {
    fprintf(stderr, "dwarf_child failed\n");
    exit(EXIT_FAILURE);
  }
  if (res == DW_DLV_NO_ENTRY) {
    DBG_PRINT("process_children: no children\n");
    goto FINISH;
  }

  tag1 = die_get_tag(die);
  tag2 = die_get_tag(child_die);
  DBG_PRINT("process_children: die tag 0x%02x, child tag 0x%02x\n", (int)tag1, (int)tag2);

  last_rec = process_record(child_die, 0);

  cur_die = child_die;

  tag1 = die_get_tag(die);
  curr = cur_record();
  prev = prev_record(curr);
  DBG_PRINT("process_children: tag 0x%02x, size %d (%d),", (int)tag1, (int)curr->size, (int)prev->size);
  DBG_PRINT("  curr_type_name='%s'",   curr->type_name ? curr->type_name : "");
  DBG_PRINT("  prev_type_name='%s'\n", prev->type_name ? prev->type_name : "");

  siz1 = prev->size;
  if (tag2 == DW_TAG_subrange_type) {
    siz2 = curr->size;
    sprintf(range_s, "%s[%u]", prev->type_name, siz2);
    free(prev->type_name);
    prev->type_name = strdup(range_s);
    prev->size *= siz2;
    DBG_PRINT("process_children: size (%d * %d) = %d, type_name='%s'\n", siz1, siz2, (int)prev->size, prev->type_name);
  }

  while (1) {
    res = dwarf_siblingof(s_dbg, cur_die, &sib_die, &error);
    if (res == DW_DLV_ERROR) {
      fprintf(stderr, "dwarf_sublingof failed\n");
      exit(EXIT_FAILURE);
    }
    if (res == DW_DLV_NO_ENTRY) {
      DBG_PRINT("process_children: no more children\n");
      break;
    }

    tag2 = die_get_tag(sib_die);
    DBG_PRINT("process_children (tags: 0x%02x, 0x%02x)\n", tag1, tag2);

    /* Don't make new record  for the DW_TAG_subrange_type */
    if (tag2 != DW_TAG_subrange_type) {
        last_rec = process_record(sib_die, 0);
    }  else {
        /* DW_TAG_subrange_type */
        DBG_PRINT("process_children DW_TAG_subrange_type\n");
        siz1 = prev->size;
        siz2 = process_subrange(sib_die);

        sprintf(range_s, "%s[%u]", prev->type_name, siz2);
        free(prev->type_name);
        prev->type_name = strdup(range_s);
        prev->size *= siz2;
        DBG_PRINT("process_children: size (%d * %d) = %d, type_name='%s'\n", siz1, siz2, (int)prev->size, prev->type_name);
    }

    if (cur_die != child_die)
      dwarf_dealloc(s_dbg, cur_die, DW_DLA_DIE);

    cur_die = sib_die;
  }
  if (cur_die != child_die)
    dwarf_dealloc(s_dbg, cur_die, DW_DLA_DIE);

  dwarf_dealloc(s_dbg, child_die, DW_DLA_DIE);

FINISH:

  DBG_PRINT("<<<<< process_children\n");
  fflush(stderr);

  return last_rec;
}

static int die_nof_children(Dwarf_Die die)
{
  Dwarf_Die child_die = NULL;
  Dwarf_Die sib_die = NULL;
  Dwarf_Die cur_die = NULL;
  Dwarf_Error error;
  int children = 0;
  int res;

  DBG_PRINT(">>>>> die_nof_children\n");

  res = dwarf_child(die, &child_die, &error);
  if (res == DW_DLV_ERROR) {
    fprintf(stderr, "dwarf_child failed\n");
    exit(EXIT_FAILURE);
  }
  if (res == DW_DLV_NO_ENTRY) {
    DBG_PRINT("die_nof_children: no children\n");
    goto FINISH;
  }

  children += 1;

  cur_die = child_die;

  while (1) {
    res = dwarf_siblingof(s_dbg, cur_die, &sib_die, &error);
    if (res == DW_DLV_ERROR) {
      fprintf(stderr, "dwarf_sublingof failed\n");
      exit(EXIT_FAILURE);
    }
    if (res == DW_DLV_NO_ENTRY) {
      DBG_PRINT("die_nof_children: no more children\n");
      break;
    }

    children += 1;

    if (cur_die != child_die)
      dwarf_dealloc(s_dbg, cur_die, DW_DLA_DIE);

    cur_die = sib_die;
  }
  if (cur_die != child_die)
    dwarf_dealloc(s_dbg, cur_die, DW_DLA_DIE);

  dwarf_dealloc(s_dbg, child_die, DW_DLA_DIE);

FINISH:

  DBG_PRINT("<<<<< die_nof_children (%d)\n", children);

  return children;
}

static dies_memb_t *cur_record(void)
{
  return &s_dies[s_dies_index-1];
}

static dies_memb_t *prev_record(dies_memb_t *rec)
{
  return rec-1;
}

static dies_memb_t *add_record(Dwarf_Die die)
{
  dies_memb_t *p = NULL;
  UNUSED_PARAM(die);

  DBG_FUNC_IN;

  p = &s_dies[s_dies_index];

  p->memb_type  = DIES_MEMB_TYPE_STRUCT;
  p->type_name  = NULL;
  p->field_name = NULL;
  p->size       = 0;
  p->offset     = 0;
  p->bit_offset = 0;
  p->bit_size   = 0;
  p->format     = FT_NONE;
  p->display    = BASE_NONE;
  p->strings    = NULL;
  p->self_id    = s_dies_index;
  p->flag       = 0;
  p->hf_id      = -1;
  p->ett_id     = -1;

  DBG_PRINT("NEW RECORD[%d]()\n", s_dies_index);

  s_dies_index += 1;

  dies_check_memory();

  DBG_FUNC_OUT;

  return &s_dies[s_dies_index-1];
}

static Dwarf_Die typedef_to_type(Dwarf_Die die)
{
  Dwarf_Die  cur_die;
  Dwarf_Die  type_die;
  Dwarf_Half tag;

  type_die = die_get_type(die);
  if (NULL == type_die) {
    fprintf(stderr, "no type for typedef die\n");
    exit(EXIT_FAILURE);
  }
  tag     = die_get_tag(type_die);
  cur_die = type_die;

  DBG_PRINT("typedef_to_type_1(0x%02x)\n", tag);

  /* Go through till first non-typedef tag */
  while (DW_TAG_typedef == tag || DW_TAG_volatile_type == tag) {
    type_die  = die_get_type(cur_die);
    tag       = die_get_tag(type_die);

    DBG_PRINT("typedef_to_type_2(0x%02x)\n", tag);

    if (type_die != cur_die)
      dwarf_dealloc(s_dbg, cur_die, DW_DLA_DIE);
    cur_die = type_die;
  }

  return cur_die;
}

static void record_format(dies_memb_t *rec, Dwarf_Die die)
{
  ftenum_t       format   = FT_NONE;
  base_display_e display  = BASE_NONE;
  Dwarf_Die      type_die = die;
  Dwarf_Half     tag      = die_get_tag(type_die);

  if (DW_TAG_typedef == tag || DW_TAG_volatile_type == tag) {
    type_die  = typedef_to_type(type_die);
    tag = die_get_tag(type_die);
  }

  if (DW_TAG_pointer_type == tag) {
    display = BASE_HEX;
    format  = FT_UINT32;
  } else if (DW_TAG_enumeration_type == tag) {
    display = BASE_HEX;
    format  = FT_UINT32;
  } else if (DW_TAG_base_type == tag) {
    char *type_name = die_get_name(type_die);

/* FIXME: Wireshark currently does not support bit fields for signed types.
 * So if there is bit_field set, it is made as unsigned_format */
#define MATCH_TYPE(type_str,type_substr,unsign_format,sign_format)  \
    if (strstr(type_str, type_substr)) {                            \
      if (strstr(type_str, "unsigned") || rec->bit_size) {          \
        format = unsign_format;                                     \
        display = BASE_DEC_HEX;                                     \
      } else {                                                      \
        format = sign_format;                                       \
        display = BASE_DEC;                                         \
      }                                                             \
      goto END_MATCH_TYPE;                                          \
    }
#define MATCH_TYPE_END                          \
    END_MATCH_TYPE:

    MATCH_TYPE(type_name, "long long", FT_UINT64, FT_INT64);
    MATCH_TYPE(type_name, "long",      FT_UINT32, FT_INT32);
    MATCH_TYPE(type_name, "short",     FT_UINT16, FT_INT16);
    MATCH_TYPE(type_name, "char",      FT_UINT8,  FT_INT8);
    MATCH_TYPE(type_name, "int",       FT_UINT32, FT_INT32);
    MATCH_TYPE(type_name, "_Bool",     FT_UINT8,  FT_INT8);
    MATCH_TYPE_END;

    DBG_PRINT("FORMAT(%s) -> (%d,%d)\n", type_name, format, display);
  }
  rec->display = display;
  rec->format  = format;
}

static void dies_check_memory(void)
{
  if (s_dies_index == s_dies_size) { /* size limit is reached, reallocate memory */
    DBG_PRINT("MEMORY REALLOCATION\n");
    s_dies_size += DIE_SIZE_STEP;
    s_dies = (dies_memb_t*)realloc(s_dies, s_dies_size * sizeof(dies_memb_t));
    if (NULL == s_dies) {
      fprintf(stderr, "realloc failed\n");
      exit(EXIT_FAILURE);
    }
  }
}


static Dwarf_Half die_get_tag(Dwarf_Die die)
{
  Dwarf_Error error;
  Dwarf_Half tag;
  int res;

  res = dwarf_tag(die, &tag, &error);
  if (res != DW_DLV_OK) {
    fprintf(stderr, "dwarf_tag failed\n");
    exit(EXIT_FAILURE);
  }

  return tag;
}

static unsigned die_attr_size(Dwarf_Die die)
{
  Dwarf_Attribute attr = 0;
  Dwarf_Unsigned udata = 0;
  Dwarf_Error error;
  unsigned data = 0;
  int res;

  DBG_FUNC_IN;

  res = dwarf_attr(die, DW_AT_byte_size, &attr, &error);
  if (res == DW_DLV_ERROR) {
    fprintf(stderr, "dwarf_attr failed\n");
    exit(EXIT_FAILURE);
  }
  if (res == DW_DLV_NO_ENTRY) {
    goto FINISH;
  }

  res = dwarf_formudata(attr, &udata, &error);
  if (res == DW_DLV_OK) {
    data = (size_t)udata;
  } else if (res == DW_DLV_ERROR) {
    fprintf(stderr, "dwarf_formudata 1 failed\n");
    exit(EXIT_FAILURE);
  }
FINISH:
  dwarf_dealloc(s_dbg, attr, DW_DLA_ATTR);

  DBG_FUNC_OUT;

  return data;
}

static int die_attr_const_value(Dwarf_Die die)
{
  Dwarf_Attribute attr = 0;
  Dwarf_Unsigned udata = 0;
  Dwarf_Signed sdata = 0;
  Dwarf_Error error;
  int data = 0;
  int res;

  res = dwarf_attr(die, DW_AT_const_value, &attr, &error);
  if (res == DW_DLV_ERROR) {
    fprintf(stderr, "dwarf_attr failed\n");
    exit(EXIT_FAILURE);
  }
  if (res == DW_DLV_NO_ENTRY) {
    goto FINISH;
  }

  res = dwarf_formsdata(attr, &sdata, &error);
  if (res == DW_DLV_OK) {
    data = (int)sdata;
  } else if (res == DW_DLV_ERROR) {
    fprintf(stderr, "dwarf_formsdata failed\n");
    res = dwarf_formudata(attr, &udata, &error);
    if (res == DW_DLV_OK) {
      data = (int)udata;
    } else if (res == DW_DLV_ERROR) {
      fprintf(stderr, "dwarf_formudata 2 failed\n");
      goto FINISH;
    }
  }
FINISH:
  dwarf_dealloc(s_dbg, attr, DW_DLA_ATTR);
  return data;
}

static unsigned die_attr_const_value_by_attr_u(Dwarf_Die die, Dwarf_Attribute attr)
{
  Dwarf_Unsigned udata = 0;
  Dwarf_Error    error;
  unsigned       data = 0;
  int            res;

  UNUSED_PARAM(die);

  res = dwarf_formudata(attr, &udata, &error);
  if (res == DW_DLV_OK) {
    data = (unsigned)udata;
  } else /* if (res == DW_DLV_ERROR) */ {
    fprintf(stderr, "dwarf_formudata 2 failed (%d)\n", res);
  }

  return data;
}

static unsigned die_attr_bit_offset(Dwarf_Die die)
{
  Dwarf_Half attr_name = DW_AT_bit_offset;
  Dwarf_Attribute attr = 0;
  Dwarf_Error error = 0;

  unsigned bit_offset = 0;
  int res;

  res = dwarf_attr(die, attr_name, &attr, &error);
  if (res == DW_DLV_ERROR) {
    fprintf(stderr, "dwarf_attr failed\n");
    exit(EXIT_FAILURE);
  }
  if (res != DW_DLV_NO_ENTRY) {
    bit_offset = die_attr_const_value_by_attr_u(die, attr);
  }

  dwarf_dealloc(s_dbg, attr, DW_DLA_ATTR);
  return bit_offset;
}

static unsigned die_attr_bit_size(Dwarf_Die die)
{
  Dwarf_Half attr_name = DW_AT_bit_size;
  Dwarf_Attribute attr = 0;
  Dwarf_Error error = 0;

  unsigned bit_size = 0;
  int res;

  res = dwarf_attr(die, attr_name, &attr, &error);
  if (res == DW_DLV_ERROR) {
    fprintf(stderr, "dwarf_attr failed\n");
    exit(EXIT_FAILURE);
  }
  if (res != DW_DLV_NO_ENTRY) {
    bit_size = die_attr_const_value_by_attr_u(die, attr);
  }

  dwarf_dealloc(s_dbg, attr, DW_DLA_ATTR);
  return bit_size;
}

static unsigned die_attr_offset(Dwarf_Die die)
{
  Dwarf_Locdesc *llbuf = 0;
  Dwarf_Locdesc **llbufarray = 0;
  Dwarf_Signed no_of_elements = 0;

  enum Dwarf_Form_Class dw_form_class = DW_FORM_CLASS_UNKNOWN;
  Dwarf_Half attr_name = DW_AT_data_member_location;
  Dwarf_Half form, directform;
  Dwarf_Half version;
  Dwarf_Half offset_size;
  Dwarf_Attribute attr = 0;
  Dwarf_Error error = 0;
  unsigned offset = 0;
  int llent = 0;
  int i;
  int res;

  res = dwarf_attr(die, attr_name, &attr, &error);
  if (res == DW_DLV_ERROR) {
    fprintf(stderr, "dwarf_attr failed\n");
    exit(EXIT_FAILURE);
  }
  if (res == DW_DLV_NO_ENTRY) {
    goto FINISH;
  }

  res = dwarf_whatform(attr, &form, &error);
  if (res != DW_DLV_OK) {
    fprintf(stderr, "dwarf_whatform failed\n");
    exit(EXIT_FAILURE);
  }

  dwarf_whatform_direct(attr, &directform, &error);

  res = dwarf_get_version_of_die(die, &version, &offset_size);
  if (res != DW_DLV_OK) {
    fprintf(stderr, "dwarf_get_version_of_die failed\n");
    exit(EXIT_FAILURE);
  }

  dw_form_class = dwarf_get_form_class(version, attr_name, offset_size, form);

  /* DWARF4 specific */
  if (dw_form_class == DW_FORM_CLASS_CONSTANT) {
    offset = die_attr_const_value_by_attr_u(die, attr);
    DBG_PRINT("offset %d (DW_FORM_CLASS_CONSTANT)\n", offset);
    goto FINISH;
  } else
  if (dw_form_class != DW_FORM_CLASS_BLOCK) {
    fprintf(stderr, "ERROR: CLASS = '0x%02x'\n", dw_form_class);
    goto FINISH;
  }

  res = dwarf_loclist_n(attr, &llbufarray, &no_of_elements, &error);
  if (res != DW_DLV_OK) {
    fprintf(stderr, "dwarf_loclist_n failed (%d)\n", res);
    exit(EXIT_FAILURE);
  }

  for (llent = 0; llent < no_of_elements; ++llent) {
    Dwarf_Half no_of_ops = 0;

    llbuf = llbufarray[llent];
    no_of_ops = llbuf->ld_cents;
    for (i = 0; i < no_of_ops; i++) {
      Dwarf_Loc *op = &llbuf->ld_s[i];
      Dwarf_Unsigned data;
      data = op->lr_number;
      offset = (unsigned)data;
      DBG_PRINT("offset %d (DW_FORM_CLASS_BLOCK)\n", offset);

      /* FIXME: unconditional break from the 2nd level loop */
      goto FINISH;
    }
  }

FINISH:
  dwarf_dealloc(s_dbg, attr, DW_DLA_ATTR);

  for (i = 0; i < no_of_elements; ++i) {
    dwarf_dealloc(s_dbg, llbufarray[i]->ld_s, DW_DLA_LOC_BLOCK);
    dwarf_dealloc(s_dbg, llbufarray[i], DW_DLA_LOCDESC);
  }
  dwarf_dealloc(s_dbg, llbufarray, DW_DLA_LIST);
  return offset;
}

static char* die_get_name(Dwarf_Die die)
{
  char *name = NULL;
  Dwarf_Error error;
  int res;

  DBG_FUNC_IN;

  res = dwarf_diename(die, &name, &error);
  if (res == DW_DLV_ERROR) {
    fprintf(stderr, "dwarf_diename failed\n");
    exit(EXIT_FAILURE);
  }

  return name;
}

static char* die_get_type_name(Dwarf_Die die)
{
  static char res[256];

  memset(res, 0, 256);
  die_get_type_name_(die, res);

  DBG_FUNC_OUT;

  return strdup(res);
}

static void die_get_type_name_(Dwarf_Die die, char *res)
{
  Dwarf_Half tag;

  DBG_FUNC_IN;

  tag = die_get_tag(die);

  if (tag == DW_TAG_base_type
      || tag == DW_TAG_structure_type
      || tag == DW_TAG_union_type
      || tag == DW_TAG_typedef) {
    char *name = NULL;
    name = die_get_name(die);
    if (name)
      FORMAT_TYPE2(res, name, "%s%s");
    if (name) dwarf_dealloc(s_dbg, name, DW_DLA_STRING);
  }
  else {
    Dwarf_Die type_die = NULL;
    switch (tag) {
    case DW_TAG_pointer_type:
      FORMAT_TYPE(res, "*%s");
      break;
    case DW_TAG_subroutine_type:
      FORMAT_TYPE(res, "%s()");
      break;
    case DW_TAG_array_type:
      /* FORMAT_TYPE(res, "%s[]"); */
      break;
    default:
      ; /* do nothing */
    }
    type_die = die_get_type(die);
    if (type_die) {
      die_get_type_name_(type_die, res);
    } else if (DW_TAG_pointer_type == tag
               || DW_TAG_subroutine_type == tag ) {
      FORMAT_TYPE(res, "void%s");
    }
    dwarf_dealloc(s_dbg, type_die, DW_DLA_DIE);
  }

  DBG_FUNC_OUT;
}

static unsigned process_subrange(Dwarf_Die die)
{
  Dwarf_Attribute attr = 0;
  Dwarf_Unsigned udata = 0;
  Dwarf_Signed sdata = 0;
  Dwarf_Error error;
  unsigned range = 0;
  int res;

  DBG_PRINT(">>>>> process_subrange\n");

  res = dwarf_attr(die, DW_AT_upper_bound, &attr, &error);
  if (res == DW_DLV_ERROR) {
    fprintf(stderr, "dwarf_attr failed\n");
    exit(EXIT_FAILURE);
  }
  if (res == DW_DLV_NO_ENTRY) {
    range = 0;
    goto FINISH_NO_ATTR;
  }

  res = dwarf_formudata(attr, &udata, &error);
  if (res == DW_DLV_OK) {
    range = (unsigned)udata + 1;
  } else if (res == DW_DLV_ERROR) {
    fprintf(stderr, "dwarf_formudata 3 failed\n");
    res = dwarf_formsdata(attr, &sdata, &error);
    if (res == DW_DLV_OK) {
      range = (unsigned)sdata + 1;
    } else if (res == DW_DLV_ERROR) {
      fprintf(stderr, "dwarf_formsdata failed\n");
      goto FINISH;
    }
  }

FINISH:
  dwarf_dealloc(s_dbg, attr, DW_DLA_ATTR);
FINISH_NO_ATTR:

  DBG_PRINT("<<<<< process_subrange\n");

  return range;
}

static Dwarf_Die die_get_type(Dwarf_Die die)
{
  Dwarf_Attribute attr = 0;
  Dwarf_Off offset = 0;
  Dwarf_Die type_die = 0;
  Dwarf_Error error;
  int res;

  res = dwarf_attr(die, DW_AT_type, &attr, &error);
  if (res == DW_DLV_ERROR) {
    fprintf(stderr, "dwarf_attr failed\n");
    exit(EXIT_FAILURE);
  }
  if (res == DW_DLV_NO_ENTRY) {
    goto FINISH;
  }

  res = dwarf_global_formref(attr, &offset, &error);
  if (res != DW_DLV_OK) {
    fprintf(stderr, "dwarf_global_formref failed\n");
    exit(EXIT_FAILURE);
  }

  res = dwarf_offdie_b(s_dbg, offset, is_info, &type_die, &error);
  if (res == DW_DLV_ERROR) {
    fprintf(stderr, "dwarf_offdie_b failed\n");
    exit(EXIT_FAILURE);
  }
  if (res == DW_DLV_NO_ENTRY) {
    goto FINISH;
  }

FINISH:
  dwarf_dealloc(s_dbg, attr, DW_DLA_ATTR);

  return type_die;
}

static unsigned die_get_size(Dwarf_Die die)
{
  Dwarf_Die cur_die = NULL;
  Dwarf_Half tag;
  unsigned size;
  cur_die = die;
  while (1) {
    Dwarf_Die type_die;

    size = die_attr_size(cur_die);
    tag  = die_get_tag(cur_die);

    if (0 != size && DW_TAG_array_type != tag) { /* skip array size, we need array element size */
      break;
    }
    else if (DW_TAG_pointer_type == tag) { /* set 4 bytes size for pointers */
      size = 4;
      break;
    }

    type_die = die_get_type(cur_die);
    if (NULL == type_die) {
      break;
    }
    if (die != cur_die)
      dwarf_dealloc(s_dbg, cur_die, DW_DLA_DIE);

    cur_die = type_die;
  }

  if (die != cur_die)
    dwarf_dealloc(s_dbg, cur_die, DW_DLA_DIE);

  return size;
}

