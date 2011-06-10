#include <htc.h>
#include <string.h>
#include "picfs.h"
#include "error.h"
#include "sd.h"

extern FS_Unit picfs_buffer[];
#define ISOPEN(fh) ((fh & ~picfs_fh_bitmap) == 0)
#define FS_BLOCK_SIZE FS_BUFFER_SIZE

char picfs_fh_bitmap;//1 = free, 0 = used

/**
 * Resolves a block id to its SD address
 */
static void picfs_getblock(char *addr, FS_Unit block_id)
{
  if(addr == NULL)
    return;
  
  addr[0] = addr[1] = addr[2] = addr[3] = 0;
  addr[2] = block_id;
  
}

static char picfs_buffer_block(FS_Unit block_id)
{
  char addr[4];
  picfs_getblock(addr,block_id);
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
  
  picfs_getblock(addr,0);
  addr[3] += FS_SuperBlock_root_block;
  SD_read(addr,&ch,1);
  if(ch == 0)
    return -PICFS_NOENT;
  
  pointer = 0;
  for(;pointer < FS_INode_pointers;pointer++)
    {
      char filename[PICFS_FILENAME_MAX];
      char entrypos = 0;
      picfs_getblock(addr,ch);
      addr[3] += FS_INode_pointers + pointer;
      SD_read(addr,&ch,1);//dir entry listing
      if(ch == 0)
	break;
      picfs_getblock(addr,ch);
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
  char inode,ptr,nextnode;
  char addr[4];

  //Is this file open?
  if(ISOPEN(fh))
    return PICFS_EBADF;

  // Get the head inode. Is it legit?
  inode = eeprom_read(fh);
  if(inode == 0)
    return PICFS_EBADF;

  // Check the size
  nextnode = eeprom_read(fh + 1);
  if(nextnode >= FS_INODE_NUM_POINTERS)
    {
      picfs_getblock(addr,inode);
      addr[3] += FS_INode_indirect;
      SD_read(addr,&ptr,1);
      if(ptr == 0)
	return 0;
      inode = ptr;
      nextnode = 0;
    }
  picfs_getblock(addr,inode);
  addr[3] = FS_INode_size;
  addr[3] += FS_INode_pointers + nextnode;
  SD_read(addr,&ptr,1);//get next pointer
  
  if(ptr == 0 )
    return 0;
  
  eeprom_write(fh+1,nextnode + 1);//increment next pointer
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

char picfs_is_open(char fh)
{
  if(ISOPEN(fh))
    return TRUE;
  return FALSE;
}
