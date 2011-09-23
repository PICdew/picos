/**
 * PICOS, PIC operating system.
 * Author: David Coss, PhD
 * Date: 1 Sept 2011
 * License: GNU Public License version 3.0 (see http://www.gnu.org)
 *
 * This file has the code behind the file system.
 */
#include "picfs.h"
#include "picfs_error.h"
#include "sd.h"
#include "io.h"
#include "sram.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>

extern void ftab_write(unsigned int address, unsigned char val);
extern char ftab_read(unsigned int address);

FS_Unit picfs_last_raw_block = 0;
char picfs_fh_bitmap = 0xff;
#define ISOPEN(fh) ((fh & ~picfs_fh_bitmap) == 0)
#define FS_BLOCK_SIZE FS_BUFFER_SIZE

#define PICFS_MTAB_BITMAP_DEFAULT 0x3f// six entries in the table
#define MTAB_ENTRY_MAX 6
#define SIZE_picfs_mtab_entry 4
char picfs_mtab_bitmap = PICFS_MTAB_BITMAP_DEFAULT;

// In order from least to most significant byte, the 
// data in eeprom for the file handle is:
// file's root inode, most significant byte of 
// next get offset, and least significant byte
// of next get offset
#define FILE_HANDLE_SIZE 3

void picfs_offset_addr(char *sd_addr, signed int offset)
{
  long large_addr;
  large_addr = sd_addr[0];large_addr <<= 8;
  large_addr += sd_addr[1];large_addr <<= 8;
  large_addr += sd_addr[2];large_addr <<= 8;
  large_addr += sd_addr[3];
  
  large_addr += offset;

  sd_addr[3] = large_addr & 0xff;large_addr >>= 8;
  sd_addr[2] = large_addr & 0xff;large_addr >>= 8;
  sd_addr[1] = large_addr & 0xff;large_addr >>= 8;
  sd_addr[0] = large_addr & 0xff;
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
		SRAM_write(fileptr,(void*)picfs_buffer,FS_BUFFER_SIZE);
		fileptr += FS_BUFFER_SIZE;
	      }
	    else if(fileptr == -2)
	      picfs_write(0);
	    else
	      IO_puts((char*)picfs_buffer);
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
  char oldpwd[SDCARD_ADDR_SIZE];
  signed char retval = 0;
  memcpy(oldpwd,picfs_pwd,SDCARD_ADDR_SIZE);
  retval = picfs_getdir(picfs_pwd,mount_point);
  if(retval != SUCCESS)
    {
      memcpy(picfs_pwd,oldpwd,SDCARD_ADDR_SIZE);
      return -1;
    }
  return 0;
}

/**
 * Resolves a block id to its SD address
 */
static signed char picfs_getblock(char *addr, FS_Unit block_id)
{
  unsigned int larger;
  char counter;
  if(addr == NULL)
    return error_return(PICFS_EINVAL);

  if(picfs_mtab_bitmap == PICFS_MTAB_BITMAP_DEFAULT)
    return error_return(PICFS_EINVAL);
  
  counter = 0;
  larger = picfs_pwd[SDCARD_ADDR_SIZE - 2];
  larger <<= 8;
  larger = picfs_pwd[SDCARD_ADDR_SIZE - 1];
  
  addr[0] = addr[1] = 0;
  if(0xffff - block_id*FS_BLOCK_SIZE < larger)
    addr[1] = 1;
  larger += block_id * FS_BLOCK_SIZE;
  addr[3] = larger & 0xff;
  larger &= 0xff00;
  addr[2] = larger >> 8;

  return SUCCESS;
}

static char picfs_buffer_block(FS_Unit block_id)
{
  char addr[SDCARD_ADDR_SIZE];
  picfs_getblock(addr,block_id);
  SD_read(addr,(void*)picfs_buffer,FS_BUFFER_SIZE);
  return SUCCESS;
}

/**
 * Returns a file handle for a file with the 
 * specified name
 *
 * Upon error, the error code is returned as a negative value.
 */
signed char picfs_open(const char *name)
{
  char addr[SDCARD_ADDR_SIZE], inode[SDCARD_ADDR_SIZE];
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
      char entrypos = 0;
      SRAM_write(SRAM_PICFS_OPEN_SWAP_ADDR,(void*)picfs_buffer,FS_BUFFER_SIZE);
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
	  SD_read(addr,(void*)picfs_buffer,ch);// filename
	  picfs_buffer[ch] = 0;
	  picfs_offset_addr(addr,ch);entrypos += ch;
	  if(strcmp((char*)picfs_buffer,name) == 0)
	    {
	      signed char fh;
	      char eeprom_addr;
	      SRAM_read(SRAM_PICFS_OPEN_SWAP_ADDR,(void*)picfs_buffer,FS_BUFFER_SIZE);// restore swapped memory
	      SD_read(addr,&ch,1);// first inode in file
	      fh = picfs_get_free_fh();//file handle
	      if(fh < 0)
		{
		  return error_return(PICFS_ENFILE);
		}
	      eeprom_addr = fh*FILE_HANDLE_SIZE;
	      ftab_write((char)eeprom_addr,ch);
	      ftab_write((char)eeprom_addr+1,0);
	      ftab_write((char)eeprom_addr+2,0);
	      return (signed char)fh;
	    }
	  picfs_offset_addr(addr,1);entrypos++;
	}
    }
   SRAM_read(SRAM_PICFS_OPEN_SWAP_ADDR,(void*)picfs_buffer,FS_BUFFER_SIZE);// restore swapped memory
  return error_return(PICFS_NOENT);

}

signed char picfs_stat(file_t fh)
{
  unsigned int size = 0;
  char addr[SDCARD_ADDR_SIZE];
  char inode,val;
  if(!ISOPEN(fh))
    return error_return(PICFS_EBADF);

  inode = ftab_read(fh);
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
  
  curr = ftab_read(eeprom_addr+1);
  curr <<= 8;
  curr |= ftab_read(eeprom_addr+2);
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
  char addr[SDCARD_ADDR_SIZE], write_overflow[2];
  // swap out the picfs_buffer
  SRAM_write(SRAM_PICFS_WRITE_SWAP_ADDR,(void*)picfs_buffer,FS_BUFFER_SIZE);
  SD_read(picfs_pwd,(void*)picfs_buffer,FS_BLOCK_SIZE);
  num_free = picfs_buffer[FS_SuperBlock_num_free_blocks];
  
  if(num_free < 2)
    return error_return(PICFS_ENOMEM);

  // If we have re-mounted the file system, figure out where the end of
  // the raw file linked list is
  if(picfs_last_raw_block == 0 && picfs_buffer[FS_SuperBlock_raw_file] != 0)
    {
      FS_Unit free_queue = picfs_buffer[FS_SuperBlock_free_queue]; 
      picfs_getblock(addr,picfs_buffer[FS_SuperBlock_raw_file]);
      SD_read(addr,(void*)picfs_buffer,FS_BLOCK_SIZE);
      while(picfs_buffer[1] != 0)
	{
	  picfs_getblock(addr,picfs_buffer[1]);
	  picfs_last_raw_block = picfs_buffer[1];
	  SD_read(addr,(void*)picfs_buffer,FS_INode_length);
	}
      
      picfs_buffer[FS_INode_pointers] = free_queue;
      SD_read(picfs_pwd,(void*)picfs_buffer,FS_BLOCK_SIZE);
    }

  //Get first raw block
  first_block = picfs_buffer[FS_SuperBlock_free_queue];
  picfs_getblock(addr,first_block);
  SD_read(addr,(void*)picfs_buffer,FS_INode_length);
  second_block = picfs_buffer[FS_INode_pointers];
  
  // Update free queue
  picfs_getblock(addr,second_block);
  SD_read(addr,(void*)picfs_buffer,FS_INode_length);
  num_free = picfs_buffer[FS_INode_pointers];// borrowing num_free to save memory
  SD_read(picfs_pwd,(void*)picfs_buffer,FS_BLOCK_SIZE);
  picfs_buffer[FS_SuperBlock_num_free_blocks] -= 2;
  picfs_buffer[FS_SuperBlock_free_queue] = num_free;
  if(picfs_last_raw_block == 0)
    picfs_buffer[FS_SuperBlock_raw_file] = first_block;
  SD_write((void*)picfs_buffer,picfs_pwd,FS_BLOCK_SIZE);
  
  // If raw has alread been written to, update the previous block so that it points to the first_block, continuing the linked list
  if(picfs_last_raw_block != 0)
    {
      picfs_getblock(addr,picfs_last_raw_block);
      SD_read(addr,(void*)picfs_buffer,2);
      picfs_buffer[1] = first_block;
      SD_write((void*)picfs_buffer,addr,2);
    }
  picfs_last_raw_block = second_block;
  
  //Save some data to the first block
  picfs_getblock(addr,first_block);
  SRAM_read(SRAM_PICFS_WRITE_SWAP_ADDR,(void*)picfs_buffer,FS_BUFFER_SIZE);// restore the swapped-out data
  memcpy(write_overflow,(void*)picfs_buffer+FS_BUFFER_SIZE-2,2);//save the last two.
  first_block = FS_BUFFER_SIZE-1;// shift the buffer
  for(;first_block > 1;first_block--)
      picfs_buffer[first_block] = picfs_buffer[first_block-2];
  picfs_buffer[0] = MAGIC_RAW;// add the raw file block header
  picfs_buffer[1] = second_block;
  SD_write((void*)picfs_buffer,addr,FS_BUFFER_SIZE);// write first block

  // write second raw block
  picfs_getblock(addr,second_block);
  memset((void*)picfs_buffer,0,FS_BUFFER_SIZE);
  picfs_buffer[0] = MAGIC_RAW;
  memcpy((void*)picfs_buffer+2,write_overflow,2);// add the last two back to the buffer
  SD_write((void*)picfs_buffer,addr,FS_BUFFER_SIZE);//write 6 to clean the pointer away
  
  return 0;
}

signed char picfs_read(file_t fh)
{
  char inode,ptr,eeprom_addr = fh*FILE_HANDLE_SIZE;
  offset_t nextnode;
  char addr[SDCARD_ADDR_SIZE];

  //Is this file open?
  if(!ISOPEN(fh))
    return error_return(PICFS_EBADF);

  // Get the head inode. Is it legit?
  inode = ftab_read(eeprom_addr);
  if(inode == 0)
    return error_return(PICFS_EBADF);
  picfs_stat(fh);
  if(picfs_buffer[ST_SIZE] < ftab_read(eeprom_addr+1))
    return error_return(PICFS_EOF);
  if(picfs_buffer[ST_SIZE] == 0)
    if(picfs_buffer[ST_SIZE + 1] < ftab_read(eeprom_addr+2))
      return error_return(PICFS_EOF);

  nextnode = ftab_read(eeprom_addr + 1);
  nextnode <<= 8;
  nextnode |= ftab_read(eeprom_addr + 2);
  
  ftab_write(eeprom_addr+1,(char)(nextnode + 1>>8) & 0xff);
  ftab_write(eeprom_addr+2,(char)((nextnode+1) & 0xff));
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

  ftab_write(eeprom_addr++,0);
  ftab_write(eeprom_addr++,0);
  ftab_write(eeprom_addr,0);
  
  picfs_free_handle(&picfs_fh_bitmap,fh);
  return 0;
}

char picfs_is_open(file_t fh)
{
  if(ISOPEN(fh))
    return TRUE;
  return FALSE;
}
