#include "picfs_error.h"
#include "utils.h"
#include "picfs.h"
#include "io.h"

#include <string.h>

signed char picos_mount(picos_size_t addr, picos_dev_t device)
{
  if(picfs_mount(addr,device) != 0)
    {
      clear_output();
      IO_puts("Could not mount ");
      IO_putd(device);
      IO_puts(" @ ");
      IO_putd(addr);
      IO_puts("\nError code");
      IO_putd(error_code);
      putch('\n');
      return -1;
    }
  return SUCCESS;
}

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
