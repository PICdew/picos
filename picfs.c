#include "picfs.h"
#include "picfs_error.h"
#include "sd.h"
#include "io.h"

#include <htc.h>
#include <string.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

extern FS_Unit picfs_buffer[];
char picfs_fh_bitmap = 0xff;
#define ISOPEN(fh) ((fh & ~picfs_fh_bitmap) == 0)
#define FS_BLOCK_SIZE FS_BUFFER_SIZE

char picfs_mtab_bitmap = 0x3f;// six entries in the table
#define MTAB_ENTRY_MAX 6
#define SIZE_picfs_mtab_entry 4

// In order from least to most significant byte, the 
// data in eeprom for the file handle is:
// file's root inode, most significant byte of 
// next get offset, and least significant byte
// of next get offset
#define FILE_HANDLE_SIZE 3



static signed char picfs_get_free_handle( char *bitmap )
{
  signed char new_fh = 0,new_fh_mask = 1;
  if(bitmap == NULL)
    return error_return(PICFS_EINVAL);
  
  if(*bitmap == 0)
    return error_return(PICFS_ENFILE);
  
  while(new_fh_mask > 0)
    {
      if((new_fh_mask & *bitmap) != 0)
	break;
      new_fh_mask <<= 1;
      new_fh++;
    }

  *bitmap &= ~((char)new_fh_mask);

  return new_fh;
}
#define picfs_get_free_fh() picfs_get_free_handle(&picfs_fh_bitmap)
#define picfs_get_free_mtab_entry() picfs_get_free_handle(&picfs_mtab_bitmap)


/**
 * Mounts the SD card fs onto the lowest available mount point.
 *
 * Returns the mount point if successful
 *
 * On error, error_code is set and -1 is returned
 */
signed char picfs_mount(const char *sd_addr)
{
  signed char mtab_entry;
  unsigned int addr;

  if(sd_addr == NULL)
    return error_return(PICFS_EINVAL);

  if(picfs_mtab_bitmap == 0)
    return error_return(PICFS_ENFILE);
  
  mtab_entry = picfs_get_free_mtab_entry();
  if(mtab_entry < 0)
    {
      error_code = -mtab_entry;
      return -1;
    }
  
  addr = SIZE_picfs_mtab_entry * mtab_entry;

  SRAM_write(addr,sd_addr,SIZE_picfs_mtab_entry);
  return mtab_entry;
}

signed char picfs_getdir(char *addr, char mount_point)
{
  mount_point++;
  if((mount_point & picfs_mtab_bitmap) == 1)
    return error_return(PICFS_EBADF);

  SRAM_read(--mount_point,addr,SIZE_picfs_mtab_entry);
  return 0;
}

signed char picfs_chdir(char mount_point)
{
  return picfs_getdir(picfs_pwd,mount_point);
}

/**
 * Resolves a block id to its SD address
 */
static void picfs_getblock(char *addr, FS_Unit block_id)
{
  if(addr == NULL)
    return;
  
  addr[0] = addr[1] = addr[2] = addr[3] = 0;
  addr[3] = block_id * FS_BLOCK_SIZE;
  
}

static char picfs_buffer_block(FS_Unit block_id)
{
  char addr[4];
  picfs_getblock(addr,block_id);
  SD_read(addr,picfs_buffer,FS_BUFFER_SIZE);
  return 0;
}

/**
 * Returns a file handle for a file with the 
 * specified name
 *
 * Upon error, the error code is returned as a negative value.
 */
signed char picfs_open(const char *name)
{
  char addr[4];
  char ch, pointer;
  if(picfs_fh_bitmap == 0)
    return error_return(PICFS_ENFILE);
  
  picfs_getblock(addr,0);
  addr[3] += FS_SuperBlock_root_block;
  SD_read(addr,&ch,1);
  if(ch == 0)
    return error_return(PICFS_NOENT);
  
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
	      SD_read(addr,&ch,1);// first inode in file
	      fh = picfs_get_free_fh();//file handle
	      if(fh < 0)
		return error_return(PICFS_ENFILE);
	      eeprom_addr = fh*FILE_HANDLE_SIZE;
	      eeprom_write((char)eeprom_addr,ch);
	      eeprom_write((char)eeprom_addr+1,0);
	      eeprom_write((char)eeprom_addr+2,0);
	      return (signed char)fh;
	    }
	  addr[3]++;entrypos++;
	}
    }

  return error_return(PICFS_NOENT);

}

signed char picfs_stat(file_t fh)
{
  unsigned int size = 0;
  char addr[4];
  char inode,val;
  if(!ISOPEN(fh))
    return error_return(PICFS_EBADF);

  inode = eeprom_read(fh);
  picfs_getblock(addr,inode);
  while(inode != 0)
    {
      if(((int)addr[3] + FS_INode_indirect) > 255)
	addr[2]++;
      addr[3] += FS_INode_indirect;
      SD_read(addr,&val,1);
      if(val == 0)
	{
	  char pointer_counter = 1;
	  if(addr[3] == 0)
	      addr[2]--;
	  addr[3]--;// FS_INode_size
	  SD_read(addr,&val,1);
	  size += val;
	  if(addr[3] >= 253)
	    addr[2]++;
	  addr[3] += 3;
	  for(;pointer_counter < FS_INODE_NUM_POINTERS;pointer_counter++)
	    {
	      SD_read(addr,&val,1);
	      if(val == 0)
		break;
	      else
		size += FS_BLOCK_SIZE;
	      if(addr[3] == 255)
		addr[2]++;
	      addr[3]++;
	    }
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

signed char picfs_seek(file_t fh, offset_t offset, char whence)
{
  offset_t curr,size;
  char eeprom_addr = fh*FILE_HANDLE_SIZE;
  if(!ISOPEN(fh))
    return error_return(PICFS_EBADF);

  curr = picfs_stat(fh);
  if(curr != 0)
    return curr;
  size = picfs_buffer[ST_SIZE];
  size <<= 8;
  size |= picfs_buffer[ST_SIZE+1];
  
  curr = eeprom_read(eeprom_addr+1);
  curr <<= 8;
  curr |= eeprom_read(eeprom_addr+2);
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
      return error_return(PICFS_EINVAL);
    }
  return 0;
}

signed char picfs_read(file_t fh)
{
  char inode,ptr,eeprom_addr = fh*FILE_HANDLE_SIZE;
  offset_t nextnode;
  char addr[4];

  //Is this file open?
  if(!ISOPEN(fh))
    return error_return(PICFS_EBADF);

  // Get the head inode. Is it legit?
  inode = eeprom_read(eeprom_addr);
  if(inode == 0)
    return error_return(PICFS_EBADF);
  picfs_stat(fh);
  if(picfs_buffer[ST_SIZE] < eeprom_read(eeprom_addr+1))
    return error_return(PICFS_EOF);
  if(picfs_buffer[ST_SIZE] == 0)
    if(picfs_buffer[ST_SIZE + 1] < eeprom_read(eeprom_addr+2))
      return error_return(PICFS_EOF);

  nextnode = eeprom_read(eeprom_addr + 1);
  nextnode <<= 8;
  nextnode |= eeprom_read(eeprom_addr + 2);
  
  eeprom_write(eeprom_addr+1,(char)(nextnode + 1>>8) & 0xff);
  eeprom_write(eeprom_addr+2,(char)((nextnode+1) & 0xff));
  while(nextnode >= FS_INODE_NUM_POINTERS)
    {
      picfs_getblock(addr,inode);
      addr[3] += FS_INode_indirect;
      SD_read(addr,&ptr,1);
      if(ptr == 0)
	return error_return(PICFS_EOF);
      inode = ptr;
      nextnode -= FS_INODE_NUM_POINTERS;
    }
  
  picfs_getblock(addr,inode);
  ptr = (char)nextnode + FS_INode_pointers;
  if(addr[3] > 255-ptr)
    addr[2]++;
  addr[3] += ptr;
  SD_read(addr,&ptr,1);
  if(ptr == 0)
    return error_return(PICFS_EOF);
  
  return picfs_buffer_block(ptr);
}

signed char picfs_close(file_t fh)
{
  char eeprom_addr = fh*FILE_HANDLE_SIZE;
  if(!ISOPEN(fh))
    return error_return(PICFS_EBADF);

  eeprom_write(eeprom_addr++,0);
  eeprom_write(eeprom_addr++,0);
  eeprom_write(eeprom_addr,0);
  
  eeprom_addr = 1 << fh;
  picfs_fh_bitmap |= eeprom_addr;
  return 0;
}

char picfs_is_open(file_t fh)
{
  if(ISOPEN(fh))
    return TRUE;
  return FALSE;
}
#if 0// working on adding these functions...
#endif
