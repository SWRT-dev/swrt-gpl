/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

main(int argc, char *argv[])
{
  FILE *ifd, *ofd;
  int i, start, cols;
  int c;
  int len;
  char *cp;

  ofd= fopen("bins.c", "w");
  if(ofd == NULL){
    printf("Can't open bins.c for writing\n");
    exit(1);
  }
//  fprintf(ofd,"#include \"file_table.h\"\n");
  for(i=1;i<argc;i++){
    ifd= fopen(argv[i], "r");
    if(ifd == NULL){
      fprintf(stderr, "File %s not found\n", argv[i]);
      exit(1);
    }
    cols= 0;
    fprintf(ofd, "static char f%d[] __initdata ={",i);
    start=1;
    len= 0;
    while((c= fgetc(ifd)) != EOF) {
      if(cols++>12){
	cols=0;
	fprintf(ofd,"\n");
      }
      if(start == 0)
	fprintf(ofd, ",");
      start= 0;
      fprintf(ofd, "0x%x", 0xff &c);
      len++;
    }
    fprintf(ofd, "};\n");
    fclose(ifd);
    printf("argv[%d]=%s(%d)\n", i, argv[i], len);
  }
  fprintf(ofd,"file_table ft[]= {\n");
  for(i=1;i<argc;i++){
    cp= strrchr(argv[i],'/');
    if(cp == NULL)
      cp= argv[i];
    else
      cp++;
    fprintf(ofd,"{\"%s\", f%d, sizeof(f%d)},\n", cp, i, i);
  }
  fprintf(ofd,"{0,0,0}\n};\n");
  exit(0);
}

