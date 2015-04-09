/* strftime example */
#include <stdio.h>      /* puts */
#include <time.h>       /* time_t, struct tm, time, localtime, strftime */

int main ()
{
  time_t rawtime;
  struct tm * timeinfo;
  char buffer [20];

  time (&rawtime);
  timeinfo = localtime (&rawtime);

  strftime (buffer,20,"%H:%M:%S.",timeinfo);
  puts (buffer);
  printf("%s",buffer);

  return 0;
}
