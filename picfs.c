#include <htc.h>
#include <string.h>
#include "picfs.h"
#include "error.h"
#include "sd.h"

extern FS_Unit picfs_buffer[];
#define ISOPEN(fh) ((fh & ~picfs_fh_bitmap) == 0)

char picfs_fh_bitmap;//1 = free, 0 = used
char picfs_buffer_block(FS_Unit block_id)
{
  char addr[4];
  addr[0] = addr[1] = addr[2] = 0;
  addr[3] = block_id*FS_BUFFER_SIZE;
  SD_read(addr,picfs_buffer,FS_BUFFER_SIZE);
  return 0;
}

static signed char picfs_get_free_fh()
{
  signed char new_fh = 1;
  if(picfs_fh_bitmap == 0)
    return -PICFS_ENFILE;
  
  while(new_fh > 0)
    {
      if((new_fh & picfs_fh_bitmap) != 0)
	break;
      new_fh <<= 1;
    }

  picfs_fh_bitmap &= ~((char)new_fh);

  if(new_fh == 1)
    new_fh--;
  
  return new_fh;
}

/**
 * Returns a file handle for a file with the 
 * specified name
 */
signed char picfs_open(const char *name)
{
  char addr[4];
  char ch, pointer;
  if(picfs_fh_bitmap == 0)
    return -PICFS_ENFILE;
  
  addr[0] = addr[1] = addr[2] = 0;
  addr[3] = FS_SuperBlock_root_block*FS_BUFFER_SIZE;
  SD_read(addr,&ch,1);
  if(ch == 0)
    return -PICFS_NOENT;
  
  pointer = 0;
  for(;pointer < FS_INode_pointers;pointer++)
    {
      char filename[PICFS_FILENAME_MAX];
      char entrypos = 0;
      addr[3] = ch + FS_INode_pointers + pointer;
      addr[3] *= FS_BUFFER_SIZE;
      SD_read(addr,&ch,1);//dir entry listing
      if(ch == 0)
	break;
      addr[3] = ch*FS_BUFFER_SIZE;
      while(entrypos < FS_BUFFER_SIZE)
	{
	  SD_read(addr,&ch,1);
	  if(ch == 0)
	    break;
	  addr[3]++;entrypos++;
	  SD_read(addr,filename,ch);
	  filename[ch] = 0;
	  addr[3] += ch;entrypos += ch;
	  if(strcmp(filename,name) == 0)
	    {
	      signed char fh;
	      addr[3]++;
	      SD_read(addr,&ch,1);// first inode in file
	      fh = picfs_get_free_fh();//file handle
	      if(fh < 0)
		return -PICFS_ENFILE;
	      eeprom_write((char)fh,ch);
	      eeprom_write((char)fh+1,0);
	      return (signed char)ch;
	    }
	  addr[3]++;entrypos++;
	}
    }

  return -PICFS_NOENT;

}

char picfs_read(char fh)
{
  char size,inode,ptr;
  char addr[4];
  if((~picfs_fh_bitmap & fh) == 0)
    return PICFS_EBADF;

  inode = eeprom_read(fh);
  if(inode == 0)
    return PICFS_EBADF;

  addr[0] = addr[1] = addr[2] = 0;
  addr[3] = inode + FS_INode_size;
  SD_read(addr,&size,1);//get size
  addr[3] = inode + FS_INode_pointers + eeprom_read(fh + 1);
  SD_read(addr,&ptr,1);//get next pointer
  
  if(size == 0 || ptr == 0 || size < eeprom_read(fh+1)*FS_BUFFER_SIZE)
    return 0;

  addr[3] = eeprom_read(fh+1);//increment next pointer
  eeprom_write(fh+1,addr[3] + 1);
  return picfs_buffer_block(ptr);
}

char picfs_close(char fh)
{
  if(ISOPEN(fh))
    return PICFS_EBADF;

  eeprom_write(fh,0);
  eeprom_write(fh+1,0);
  picfs_fh_bitmap |= fh;
  return 0;
}

char picfs_rewind(char fh)
{
  if(ISOPEN(fh))
    return PICFS_EBADF;
  
  eeprom_write(fh+1,0);
  return 0;
}

char picfs_is_open(char fh)
{
  if(ISOPEN(fh))
    return TRUE;
  return FALSE;
}
