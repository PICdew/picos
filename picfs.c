#include <htc.h>
#include <string.h>
#include "picfs.h"
#include "error.h"
#include "sd.h"

extern FS_Unit picfs_buffer[];
#define ISOPEN(fh) ((fh & ~picfs_fh_bitmap) == 0)
#define FS_BLOCK_SIZE FS_BUFFER_SIZE

// In order from least to most significant byte, the 
// data in eeprom for the file handle is:
// file's root inode, most significant byte of 
// next get offset, and least significant byte
// of next get offset
#define FILE_HANDLE_SIZE 3

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
	      char eeprom_addr;
	      addr[3]++;
	      SD_read(addr,&ch,1);// first inode in file
	      fh = picfs_get_free_fh();//file handle
	      if(fh < 0)
		return -PICFS_ENFILE;
	      eeprom_addr = fh*FILE_HANDLE_SIZE;
	      eeprom_write((char)eeprom_addr,ch);
	      eeprom_write((char)eeprom_addr+1,0);
	      eeprom_write((char)eeprom_addr+2,0);
	      return (signed char)fh;
	    }
	  addr[3]++;entrypos++;
	}
    }

  return -PICFS_NOENT;

}

char picfs_stat(file_t fh)
{
  unsigned int size = 0;
  char addr[4];
  char inode,val;
  if(!ISOPEN(fh))
      return PICFS_EBADF;

  inode = eeprom_read(fh);
  picfs_getblock(addr,inode);
  while(inode != 0)
    {
      addr[3] = FS_INode_indirect;
      SD_read(addr,&val,1);
      if(val == 0)
	{
	  addr[3] = FS_INode_size;
	  SD_read(addr,&val,1);
	  size += val;
	  break;
	}
      size += FS_INODE_NUM_POINTERS*FS_BLOCK_SIZE;
      inode = val;
    }
  inode = 0;
  for(;inode < FS_BUFFER_SIZE;inode++)
    picfs_buffer[inode] = 0;
  
  picfs_buffer[ST_SIZE] = (char)((size >>8) & 0xff);
  picfs_buffer[ST_SIZE+1] = (char)(size & 0xff);

  return 0;
}

char picfs_seek(file_t fh, offset_t offset, char whence)
{
  offset_t curr,size;
  if(!ISOPEN(fh))
    return PICFS_EBADF;

  curr = picfs_stat(fh);
  if(curr != 0)
    return (char)curr;
  size = picfs_buffer[ST_SIZE];
  size <<= 8;
  size |= picfs_buffer[ST_SIZE+1];
  
  curr = eeprom_read(fh+1);
  curr <<= 8;
  curr |= eeprom_read(fh+2);
  //check to see if offset(whence) is past EOF
  switch(whence)
    {
    case PICFS_SET:
      curr = offset;
      break;
    case PICFS_CURR:
      curr += offset;
      break;
    case PICFS_END:
      curr = size + offset;
      break;
    case PICFS_REVERSE:
      if(offset > size)
	curr = 0;
      else
	curr = size - offset;
    default:
      return PICFS_EINVAL;
    }
  return 0;
}

char picfs_read(file_t fh)
{
  char inode,ptr;
  offset_t nextnode;
  char addr[4];

  //Is this file open?
  if(ISOPEN(fh))
    return PICFS_EBADF;

  // Get the head inode. Is it legit?
  inode = eeprom_read(fh);
  if(inode == 0)
    return PICFS_EBADF;
  picfs_stat(fh);
  if(picfs_buffer[ST_SIZE] < eeprom_read(fh+1))
    return 0;//beyond EOF
  if(picfs_buffer[ST_SIZE] == 0)
    if(picfs_buffer[ST_SIZE + 1] < eeprom_read(fh+2))
      return 0;//beyond EOF

  nextnode = eeprom_read(fh + 1);
  nextnode <<= 8;
  nextnode |= eeprom_read(fh + 2);
  
  eeprom_write(fh+1,(char)(nextnode + 1>>8) & 0xff);
  eeprom_write(fh+2,(char)((nextnode+1) & 0xff));
  while(nextnode >= FS_BLOCK_SIZE*FS_INODE_NUM_POINTERS)
    {
      picfs_getblock(addr,inode);
      addr[3] += FS_INode_indirect;
      SD_read(addr,&ptr,1);
      if(ptr == 0)
	return 0;
      inode = ptr;
      nextnode -= FS_BLOCK_SIZE*FS_INODE_NUM_POINTERS;
    }
  
  picfs_getblock(addr,inode);
  addr[3] += (char)nextnode/FS_BLOCK_SIZE;
  SD_read(addr,&inode,1);
  if(ptr == 0)
    return 0;
  
  return picfs_buffer_block(ptr);
}

char picfs_close(file_t fh)
{
  if(ISOPEN(fh))
    return PICFS_EBADF;

  eeprom_write(fh,0);
  eeprom_write(fh+1,0);
  eeprom_write(fh+2,0);
  picfs_fh_bitmap |= fh;
  return 0;
}

char picfs_is_open(file_t fh)
{
  if(ISOPEN(fh))
    return TRUE;
  return FALSE;
}
