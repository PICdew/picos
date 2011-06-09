#include "picfs.h"
#include "sd.h"

extern FS_Unit picfs_buffer[];

char picfs_buffer_block(FS_Unit block_id)
{
  char addr[4];
  addr[0] = addr[1] = addr[2] = 0;
  addr[3] = block_id*FS_BUFFER_SIZE;
  SD_read(addr,picfs_buffer,FS_BUFFER_SIZE);
  return 0;
}
