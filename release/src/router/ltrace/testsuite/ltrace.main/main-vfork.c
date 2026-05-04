#include <unistd.h>

extern void print (char *);

#define	PRINT_LOOP	10

void
th_main (char * arg)
{
  int i;
  for (i=0; i<PRINT_LOOP; i++)
    print (arg);
}

int main (int argc, char ** argv)
{
  if (argc != 1)
    {
      th_main ("aaa");
      return 0;
    }

  if (!vfork ())
    execlp (argv[0], argv[0], "", NULL);
  th_main ("bbb");

  return 0;
}
