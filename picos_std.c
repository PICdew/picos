/**
 * PICOS, PIC operating system.
 * Author: David Coss, PhD
 * Date: 1 Sept 2011
 * License: GNU Public License version 3.0 (see http://www.gnu.org)
 *
 * Utility functions used within the operating system
 */

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
