#include "picfs_error.h"
#include "utils.h"
#include "picfs.h"
#include "io.h"

#include <string.h>

signed char picos_mount(picos_size_t addr)
{
  char sd_addr[SDCARD_ADDR_SIZE];
  memset(sd_addr,0,SDCARD_ADDR_SIZE);
  sd_addr[SDCARD_ADDR_SIZE - 1] = addr & 0xff;
  addr >>= 8;
  sd_addr[SDCARD_ADDR_SIZE - 2] = addr & 0xff;
  
  if(picfs_mount(sd_addr) != 0)
    {
      clear_output();
      IO_puts("Could not mount ");
      IO_putd(addr);
      IO_puts("\nError code");
      IO_putd(error_code);
      putch('\n');
      return -1;
    }
  return SUCCESS;
}

signed char picos_chdir(picos_size_t addr)
{
  if(picfs_chdir((addr & 0xff)) != 0)
    {
      clear_output();
      IO_puts("Could not cd to /sd");
      IO_putd(addr);
      IO_puts("\nError code ");
      IO_putd(error_code);
      putch('\n');
      return -1;
    }
  return SUCCESS;
}
