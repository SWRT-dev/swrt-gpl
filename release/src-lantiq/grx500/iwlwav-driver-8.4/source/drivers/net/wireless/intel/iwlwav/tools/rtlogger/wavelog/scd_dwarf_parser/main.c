//WIN32;_DEBUG;_LIB;__LIBELF_INTERNAL__=1;%(PreprocessorDefinitions)
//WIN32;_DEBUG;_LIB;HAVE_CONFIG_H;%(PreprocessorDefinitions)

#include <stdio.h>
#include <stdlib.h>

#define SCD_DWARF_STANDALONE
#include "scd_dwarf_parser.h"

static void print(scd_evt_t *ids, unsigned ids_size, dies_memb_t *dies, unsigned dies_size);

int main()
{
  int res;
  scd_evt_t *ids;
  unsigned ids_size;
  dies_memb_t *dies;
  unsigned dies_size;

  //char* scd_fn = "fw.scd";
  //char* elf_fn = "fw.bin";

  char* scd_fn = "..\\..\\third_party\\dwarf2diss\\fw_scd_file.scd";
  char* elf_fn = "..\\..\\third_party\\dwarf2diss\\ap_upper_wave300";


  res = dwarf2_init();
  if (res != RESULT_OK) {
    return EXIT_FAILURE;
  };

  res = dwarf2_process(scd_fn, elf_fn, &ids, &ids_size, &dies, &dies_size);
  if (res != RESULT_OK) {
    return EXIT_FAILURE;
  };

  print(ids, ids_size, dies, dies_size);

  /* res = dwarf2_finish(); */
  if (res != RESULT_OK) {
    return EXIT_FAILURE;
  };

  return EXIT_SUCCESS;
}

void print(scd_evt_t *ids, unsigned ids_size, dies_memb_t *dies, unsigned dies_size)
{
  int i;
  printf("IDS:\n");
  for (i = 0; i < ids_size; ++i) {
    printf("{ %2d, %2d, %2d, %2d, %48s %16lx }\n", ids[i].oid, ids[i].gid, ids[i].fid, ids[i].lid, ids[i].name, (unsigned long)ids[i].die);
  };
  printf("DIEs:\n");
  for (i = 0; i < dies_size; ++i) {
    printf("{ %2d, %48s, %48s, %4u, %4u, %2u, %2u, %8lx, %2d, %2d, %2d, %2d }\n", dies[i].memb_type, dies[i].type_name, dies[i].field_name, dies[i].offset, dies[i].size, dies[i].format, dies[i].display, (unsigned long)dies[i].strings, dies[i].self_id, dies[i].flag, dies[i].hf_id, dies[i].ett_id);
  };
}

