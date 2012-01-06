#include "picfs_error.h"
#include "utils.h"
#include "picfs.h"
#include "io.h"

#include <string.h>

unsigned int millisleep(unsigned int ms)
{
  unsigned int usec;
  while(ms > 0)
    {
      usec = (unsigned int) ((_XTAL_FREQ >> 2)/1000L);
      while(usec > 0)
	usec--;
    }
  return ms;// in case I make this interruptable in the future. Trying to follow "sleep(unsigned int)"'s prototype
}
