#include "picfs.h"
#include "picfs_error.h"
#include "sd.h"
#include "io.h"

#include <htc.h>
#include <string.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

FS_Unit picfs_last_raw_block = 0;
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

void picfs_offset_addr(char *sd_addr, signed int offset)
{
  signed long large_addr;
  if(offset == 0)
    return;
  if(sd_addr == NULL)
    return;
  large_addr = sd_addr[3];
  large_addr += sd_addr[2] << 8;
  large_addr += sd_addr[1] << 16;
  large_addr += sd_addr[0] << 24;
  
  large_addr += offset;
  
  sd_addr[3] = large_addr & 0xff;
  sd_addr[2] = (large_addr & 0xff00) >> 8;
  sd_addr[1] = (large_addr & 0xff0000) >> 16;
  sd_addr[0] = (large_addr & 0xff000000) >> 24;
}

void cat_file(const char *filename, int fileptr)
{
  signed char retval;
  file_t file;
  retval = picfs_open(filename);
  if(retval < 0)
    {
      IO_puts("Could not open ");
      IO_puts(filename);
      IO_putd(error_code);
      error_code = 0;
    }
  file = (file_t)retval;
  if(picfs_is_open(file))
    {
      if(fileptr == -1)
	{
	  IO_puts(filename);
	  IO_puts(":\n");
	}
      while(TRUE)
	{
	if(picfs_read(file) != 0)
	  {
	    if(error_code == PICFS_EOF)
		error_code = 0;
	    break;
	  }
	else
	  {
	    if(fileptr >= 0)
	      {
		SRAM_write(fileptr,picfs_buffer,FS_BUFFER_SIZE);
		fileptr += FS_BUFFER_SIZE;
	      }
	    else if(fileptr == -2)
	      picfs_write(0);
	    else
	      IO_puts(picfs_buffer);
	  }
	}
      picfs_close(file);
      if(fileptr == -1)
	putch('\n');
    }
  else
    {
      IO_puts("Could not open startup");
    }
}

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


signed char picfs_verify_fs(const char *sd_addr)
{
  char magic_numbers[FS_SuperBlock_block_size+1];
  SD_read(sd_addr,magic_numbers,FS_SuperBlock_block_size+1);
  if(magic_numbers[0] != 0 && magic_numbers[1] != 0x6 && magic_numbers[2] != 0x29 && magic_numbers[3] != 0x83)
    {
      return error_return(PICFS_INVALID_FILESYSTEM);
    }
  if(magic_numbers[FS_SuperBlock_block_size] != FS_BLOCK_SIZE)
    return error_return( PICFS_INVALID_FILESYSTEM/*FS_BLOCK_SIZE */);
  
  return SUCCESS;
}

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
    return -1;// error_code already set by previous function

  addr = SIZE_picfs_mtab_entry * mtab_entry;

  if(picfs_verify_fs(sd_addr) != 0)
    {
      void picfs_free_handle(char *bitmap, file_t fh);
      picfs_free_handle(&picfs_mtab_bitmap,mtab_entry);
      return -1;// error_code already set by previous function
    }
  
  SRAM_write(addr,sd_addr,SIZE_picfs_mtab_entry);
  return mtab_entry;
}

static signed char picfs_getdir(char *addr, char mount_point)
{
  char mount_point_mask = 1 << mount_point;
  if((mount_point_mask & picfs_mtab_bitmap) != 0)
    return error_return(PICFS_EBADF);

  SRAM_read(mount_point*SIZE_picfs_mtab_entry,addr,SIZE_picfs_mtab_entry);
  return 0;
}

signed char picfs_chdir(char mount_point)
{
  char oldpwd[4];
  signed char retval = 0;
  memcpy(oldpwd,picfs_pwd,4);
  retval = picfs_getdir(picfs_pwd,mount_point);
  if(retval != SUCCESS)
    {
      memcpy(picfs_pwd,oldpwd,4);
      return -1;
    }
  return 0;
}

/**
 * Resolves a block id to its SD address
 */
static void picfs_getblock(char *addr, FS_Unit block_id)
{
  unsigned int larger;
  if(addr == NULL)
    return;
  
  addr[0] = addr[1] = 0;
  larger = block_id * FS_BLOCK_SIZE;
  addr[3] = larger & 0xff;
  larger &= 0xff00;
  addr[2] = larger >> 8;
  
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
  char addr[4], inode[4];
  char ch, pointer;
  if(picfs_fh_bitmap == 0)
    return error_return(PICFS_ENFILE);
  
  picfs_getblock(inode,0);
  picfs_offset_addr(inode,FS_SuperBlock_root_block);
  SD_read(inode,&ch,1);
  if(ch == 0)
    return error_return(PICFS_NOENT);
  
  picfs_getblock(inode,ch); // now contains inode's address
  picfs_offset_addr(inode,FS_INode_pointers);// now contains the first pointer
  pointer = 0;
  for(;pointer < FS_INode_pointers;pointer++,picfs_offset_addr(inode,1))
    {
      char filename[PICFS_FILENAME_MAX];
      char entrypos = 0;
      SD_read(inode,&ch,1);//dir entry listing
      if(ch == 0)
	break;
      picfs_getblock(addr,ch);
      while(entrypos < FS_BUFFER_SIZE)
	{
	  SD_read(addr,&ch,1);
	  if(ch == 0)
	    break;
	  picfs_offset_addr(addr,1);entrypos++;
	  SD_read(addr,filename,ch);
	  filename[ch] = 0;
	  picfs_offset_addr(addr,ch);entrypos += ch;
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
	  picfs_offset_addr(addr,1);entrypos++;
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
      picfs_offset_addr(addr,FS_INode_indirect);
      SD_read(addr,&val,1);
      if(val == 0)
	{
	  char pointer_counter = 1;
	  picfs_offset_addr(addr,-1);// FS_INode_size
	  SD_read(addr,&val,1);
	  size += val;
	  picfs_offset_addr(addr,3);
	  for(;pointer_counter < FS_INODE_NUM_POINTERS;pointer_counter++)
	    {
	      SD_read(addr,&val,1);
	      if(val == 0)
		break;
	      else
		size += FS_BLOCK_SIZE;
	      picfs_offset_addr(addr,1);
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

signed char picfs_write(file_t fh)
{
  char num_free,first_block,second_block;
  char addr[4], buffer[FS_BLOCK_SIZE];
  SD_read(picfs_pwd,buffer,FS_BLOCK_SIZE);
  num_free = buffer[FS_SuperBlock_num_free_blocks];
  
  if(num_free < 2)
    return error_return(PICFS_ENOMEM);

  // If we have re-mounted the file system, figure out where the end of
  // the raw file linked list is
  if(picfs_last_raw_block == 0 && buffer[FS_SuperBlock_raw_file] != 0)
    {
      FS_Unit free_queue = buffer[FS_SuperBlock_free_queue]; 
      picfs_getblock(addr,buffer[FS_SuperBlock_raw_file]);
      SD_read(addr,buffer,FS_BLOCK_SIZE);
      while(buffer[1] != 0)
	{
	  picfs_getblock(addr,buffer[1]);
	  picfs_last_raw_block = buffer[1];
	  SD_read(addr,buffer,FS_INode_length);
	}
      
      buffer[FS_INode_pointers] = free_queue;
      SD_read(picfs_pwd,buffer,FS_BLOCK_SIZE);
    }

  //Get first raw block
  first_block = buffer[FS_SuperBlock_free_queue];
  picfs_getblock(addr,first_block);
  SD_read(addr,buffer,FS_INode_length);
  second_block = buffer[FS_INode_pointers];
  
  // Update free queue
  picfs_getblock(addr,second_block);
  SD_read(addr,buffer,FS_INode_length);
  num_free = buffer[FS_INode_pointers];// borrowing num_free to save memory
  SD_read(picfs_pwd,buffer,FS_BLOCK_SIZE);
  buffer[FS_SuperBlock_num_free_blocks] -= 2;
  buffer[FS_SuperBlock_free_queue] = num_free;
  if(picfs_last_raw_block == 0)
    buffer[FS_SuperBlock_raw_file] = first_block;
  SD_write(buffer,picfs_pwd,FS_BLOCK_SIZE);
  
  // If raw has alread been written to, update the previous block so that it points to the first_block, continuing the linked list
  if(picfs_last_raw_block != 0)
    {
      picfs_getblock(addr,picfs_last_raw_block);
      SD_read(addr,buffer,2);
      buffer[1] = first_block;
      SD_write(buffer,addr,2);
    }
  picfs_last_raw_block = second_block;
  
  //Save some data to the first block
  picfs_getblock(addr,first_block);
  buffer[0] = MAGIC_RAW;
  buffer[1] = second_block;
  memcpy(buffer + 2,picfs_buffer,FS_BLOCK_SIZE - 2);
  SD_write(buffer,addr,FS_BLOCK_SIZE);

  // write second raw block
  picfs_getblock(addr,second_block);
  memset(buffer,0,FS_BLOCK_SIZE);
  buffer[0] = MAGIC_RAW;
  memcpy(buffer+2,picfs_buffer + FS_BLOCK_SIZE - 2,2);
  SD_write(buffer,addr,6);//write 6 to clean the pointer away
    
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
      picfs_offset_addr(addr,FS_INode_indirect);
      SD_read(addr,&ptr,1);
      if(ptr == 0)
	return error_return(PICFS_EOF);
      inode = ptr;
      nextnode -= FS_INODE_NUM_POINTERS;
    }
  
  picfs_getblock(addr,inode);
  ptr = (char)nextnode + FS_INode_pointers;
  picfs_offset_addr(addr,ptr);
  SD_read(addr,&ptr,1);
  if(ptr == 0)
    return error_return(PICFS_EOF);
  
  return picfs_buffer_block(ptr);
}

void picfs_free_handle(char *bitmap, file_t fh)
{
  char mask = 1 << fh;
  if(bitmap != NULL)
    *bitmap |= mask;
}

signed char picfs_close(file_t fh)
{
  char eeprom_addr = fh*FILE_HANDLE_SIZE;
  if(!ISOPEN(fh))
    return error_return(PICFS_EBADF);

  eeprom_write(eeprom_addr++,0);
  eeprom_write(eeprom_addr++,0);
  eeprom_write(eeprom_addr,0);
  
  picfs_free_handle(&picfs_fh_bitmap,fh);
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
