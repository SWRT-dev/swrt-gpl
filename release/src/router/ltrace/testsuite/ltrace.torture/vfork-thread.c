#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>


void *
routine (void *data)
{
  int i;
  for (i = 0; i < 6; ++i)
    {
      puts ("bleble");
      sleep (1);
    }
  return NULL;
}


void *
routine2 (void *data)
{
  pid_t child = vfork ();
  if (child == 0)
    {
      int i, j;
      puts ("vforked");
      for (i = 0; i < 100000; ++i)
	for (j = 0; j < 10000; ++j)
	  ;
      puts ("vforked child exiting");
      _exit (0);
    }
  puts ("parent continuing");
  return NULL;
}

int
main(int argc, char *argv[])
{
  pthread_t thread;
  pthread_create (&thread, NULL, &routine, NULL);

  sleep (1);

  pthread_t thread2;
  pthread_create (&thread2, NULL, &routine2, NULL);
  pthread_join (thread2, NULL);
  pthread_join (thread, NULL);
  return 0;
}
