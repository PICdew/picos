#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>
#include "fs.h"
#include "pasm.h"

#include <ctype.h>
#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <fuse.h>
#include <libgen.h>
#include <limits.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <math.h>
#include <sys/types.h>
#include <sys/xattr.h>

static void log_msg(const char *format, ...)
{
    va_list ap;
    if(FS_PRIVATE_DATA->verbose_log == TRUE)
      {
	va_start(ap, format);
	vfprintf(FS_PRIVATE_DATA->logfile, format, ap);
      }
}

static void error_log(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    vfprintf(FS_PRIVATE_DATA->logfile, format, ap);
}

static FS_Block* FS_getblock(FS_Block *super_block, FS_Unit block_id)
{
  return &(super_block[block_id*FS_BLOCK_SIZE]);
}


static void FS_free_block(FS_Block *sb, FS_Unit block_id)
{
  FS_Block *free_block;
  if(sb == NULL || block_id >= sb[FS_SuperBlock_num_blocks])
    return;
  free_block = FS_getblock(sb,block_id);
  log_msg("Freeing contents of block %x\n",block_id);
  memset(free_block,0,FS_BLOCK_SIZE*sizeof(FS_Unit));
  free_block[FS_INode_magic_number] = MAGIC_FREE_INODE;
  free_block[FS_INode_pointers] = sb[FS_SuperBlock_free_queue];
  sb[FS_SuperBlock_free_queue] = block_id;
  sb[FS_SuperBlock_num_free_blocks]++;
}

static FS_Unit FS_malloc(FS_Block *sb)
{
  FS_Block retval = sb[FS_SuperBlock_free_queue], *new_node=NULL;
  if(sb[FS_SuperBlock_num_free_blocks] == 0 || retval == 0)
      return retval;

  new_node = FS_getblock(sb,retval);
  log_msg("malloced %d\n",retval);
  sb[FS_SuperBlock_free_queue] = new_node[FS_INode_pointers];
  sb[FS_SuperBlock_num_free_blocks]--;
  memset(new_node,0,FS_BLOCK_SIZE*sizeof(FS_Unit));
  return retval;
}

static int FS_is_virtual(const char *path)
{
  return (strcmp(path,"/") == 0) 
    || (strcmp(path,"/dump") == 0) 
    || (strcmp(path,"/eeprom") == 0)
    || (strcmp(path,"/picc") == 0);
}

static int FS_compile(FS_Block *sb,FILE *eeprom_file, int type)
{
  char hex_buffer[45];
  struct compiled_code *code = NULL, *code_end = NULL;
  int len = 0;
  size_t num_bytes,curr_byte = 0;
  if(sb == NULL)
    return 0;
  if(eeprom_file == NULL)
    eeprom_file = FS_PRIVATE_DATA->logfile;
  
  num_bytes = sb[FS_SuperBlock_block_size]*sb[FS_SuperBlock_num_blocks];
  
  for(;curr_byte < num_bytes;curr_byte++)
    insert_compiled_code(&code,&code_end,sb[curr_byte]);
  
  memset(hex_buffer,0,(9 + COMPILE_MAX_WIDTH + 2)*sizeof(char));
  FPrintCode(eeprom_file,code,0,hex_buffer,0x4200,0,type);
  FreeCode(code);

  if(eeprom_file != FS_PRIVATE_DATA->logfile)
    {
      fseek(eeprom_file,0,SEEK_END);
      len = ftell(eeprom_file);
      rewind(eeprom_file);
    }
  else
    len = 0;
  //  eeprom = (FS_Block*)malloc(num_bytes);
  //  eeprom = fgets(eeprom,num_bytes,eeprom_file);
  
  //  fclose(eeprom_file);

  return len;
}

static int FS_read_eeprom(FS_Block *sb,FILE *eeprom_file)
{
  int len;
  fputs(":020000040000FA\n",eeprom_file);
  FS_compile(sb,eeprom_file, PRINT_HEX);
  fputs(":00000001FF\n",eeprom_file);
  if(eeprom_file != FS_PRIVATE_DATA->logfile)
    {
      fseek(eeprom_file,0,SEEK_END);
      len = ftell(eeprom_file);
      rewind(eeprom_file);
    }
  else
    len = 0;
  
  return len;
}

static int FS_read_picc(FS_Block *sb,FILE *c_file)
{
  int len;
  FS_compile(sb,c_file, PRINT_EEPROM_DATA);
  if(c_file != FS_PRIVATE_DATA->logfile)
    {
      fseek(c_file,0,SEEK_END);
      len = ftell(c_file);
      rewind(c_file);
    }
  else
    len = 0;
  
  return len;
}

void FS_mksuperblock(FS_Block *block, struct fs_fuse_state *the_state)
{
  size_t num_blocks,block_size;
  if(block == NULL || the_state == NULL)
    return;

  num_blocks = the_state->num_blocks;
  block_size = the_state->block_size;
  block[FS_SuperBlock_magic_number] = MAGIC_SUPERBLOCK;
  block[FS_SuperBlock_revision_num] = FS_REVISION_NUM;
  block[FS_SuperBlock_block_size] = block_size;
  block[FS_SuperBlock_num_blocks] = num_blocks;
  block[FS_SuperBlock_num_free_blocks] = block[FS_SuperBlock_num_blocks] - 1;
  block[FS_SuperBlock_root_block] = 1;

  block[FS_SuperBlock_free_queue] = 2;
}

void FS_mkinode(FS_Block *inode, FS_Unit block_size)
{
  size_t i;
  size_t num_pointers = block_size - FS_INode_length;
  if(inode == NULL)
    return;

  inode[FS_INode_magic_number] = MAGIC_DATA;
  inode[FS_INode_uid] = 0;
  inode[FS_INode_mode] = 075;
  inode[FS_INode_size] = 0;
  inode[FS_INode_pointers] = 0;
  i = 0;
  for(;i<num_pointers;i++)
    inode[FS_INode_pointers + i] = 0;
}

int FS_allocate(FS_Block **data,size_t num_blocks,size_t block_size)
{
  if(data == NULL)
    return -1;

  *data = (FS_Block*)realloc(*data,num_blocks*block_size*sizeof(FS_Unit));
  return 0;
}

static int FS_file_size(FS_Block *the_dir)
{
  FS_Block *sb = FS_PRIVATE_DATA->super_block;
  size_t datacount = 0, filesize = 0;
   
  if(the_dir == NULL)
    return 0;
  while(1)
    {
      if(the_dir[FS_INode_indirect] == 0)
	{
	  for(;datacount < FS_INODE_NUM_POINTERS;datacount++)
	    if(the_dir[FS_INode_pointers + datacount] == 0)
	      break;
  
	  if(datacount != 0)
	    filesize += (datacount-1)*FS_BLOCK_SIZE + the_dir[FS_INode_size];
	  break;
	}
      else
	{
	  filesize += FS_BLOCK_SIZE*FS_INODE_NUM_POINTERS;
	}
      the_dir = FS_getblock(sb,the_dir[FS_INode_indirect]);
    }

  return filesize;
  
}


static void FS_inode2stat(struct stat *stbuf, const FS_Block *the_dir)
{
  FS_Block *sb = FS_PRIVATE_DATA->super_block;
  log_msg("inode2stat\n");
  if(stbuf == NULL || the_dir == NULL)
    return;

  stbuf->st_mode = ((the_dir[FS_INode_mode] & 070) << 3) | (the_dir[FS_INode_mode]);
  switch(the_dir[FS_INode_magic_number])
    {
    case MAGIC_DIR:
      stbuf->st_mode |= S_IFDIR;
      stbuf->st_size = the_dir[FS_INode_size];
      break;
    case MAGIC_DATA:default:
      stbuf->st_mode |= S_IFREG;
      stbuf->st_size = FS_file_size(the_dir);
      break;
    }
    
  stbuf->st_nlink = 1;
}


static FS_Block* FS_resolve(FS_Block *dir, const char *path, FS_Block *sb)
{
  char *rw_path;
  char *token;
  size_t num_entries,dir_counter;
  FS_Block *dirent;
  int moved_up = FALSE;
  
  if(dir == NULL || path == NULL || dir[FS_INode_magic_number] != MAGIC_DIR)
    return NULL;

  rw_path = (char*)malloc((strlen(path)+1)*sizeof(char));
  strcpy(rw_path,path);
  token = strtok(rw_path,"/");
  log_msg("%s/\n",token);
  while(token != NULL)
    {
      moved_up = FALSE;
      num_entries = dir[FS_INode_size];
      dir_counter = 0;
      for(;dir_counter<num_entries;dir_counter++)
	{
	  dirent = FS_getblock(sb,dir[FS_INode_pointers + dir_counter]);
	  while(dirent != NULL && dirent[0] != 0)
	    {
	      size_t len = (size_t)dirent[0];dirent++;
	      FS_Unit d_name[FS_BLOCK_SIZE];
	      memcpy(d_name,dirent,len);
	      d_name[len] = 0;dirent += len;
	      log_msg("%s ?= %s\n",d_name,token);
	      if(strcmp(d_name,token) == 0)
		{
		  dir = FS_getblock(sb,dirent[0]);
		  moved_up = TRUE;
		  if(dir[FS_INode_magic_number] != MAGIC_DIR)
		    {
		      free(rw_path);
		      log_msg("Found %s at 0x%x\n",path,dir[0]);
		      return dir;
		    }
		  break;
		}
	      dirent++;
	    }
	  if(moved_up)
	    {
	      token = strtok(NULL,"/");
	      break;
	    }
	}
      if(!moved_up)
	return NULL;      
    }
  
  free(rw_path);
  if(dir != NULL)
    log_msg("Found %s\n",path);
  else
    log_msg("Could not find %s\n",path);
  
  return dir;
}


static int fs_getattr(const char *path, struct stat *stbuf)
{
    int res = 0;
    FS_Block *sb = FS_PRIVATE_DATA->super_block;
    FS_Block *dir = FS_getblock(sb,sb[FS_SuperBlock_root_block]);

    log_msg("fs_getattr (%s)\n",path);

    if(strcmp(path,"/") == 0)
      FS_inode2stat(stbuf,dir);

    if(strcmp(path,"/dump") == 0)
      {
	stbuf->st_mode = 0555 | S_IFREG;
	stbuf->st_size = sb[FS_SuperBlock_num_blocks] * sb[FS_SuperBlock_block_size];
	return 0;
      }
    else if(strcmp(path,"/eeprom") == 0)
      {
	FILE *eeprom = tmpfile();
	int len = FS_read_eeprom(sb,eeprom);
	stbuf->st_mode = 0555 | S_IFREG;
	stbuf->st_size = len;
	fclose(eeprom);
	return 0;
      }
    else if(strcmp(path,"/picc") == 0)
      {
	FILE *picc = tmpfile();
	int len = FS_read_picc(sb,picc);
	stbuf->st_mode = 0555 | S_IFREG;
	stbuf->st_size = len;
	fclose(picc);
	return 0;
      }
    dir = FS_resolve(dir,path,sb);
    if(dir == NULL)
      return -ENOENT;
    
    FS_inode2stat(stbuf,dir);
    
    return res;
}

static int FS_access(const char *path, int mode)
{
  struct stat status;
  int retval = fs_getattr(path,&status);
  mode_t good_mode = 07;
  log_msg("FS_access %s mode 0%o\n",path,mode);
  if(retval != 0)
      return retval;
  
  if(status.st_uid == getuid())
    good_mode |= 0700;
  if(status.st_gid == getgid())
    good_mode |= 070;

  if(mode & R_OK != 0 && ((good_mode && 0444) & status.st_mode == 0))
    return -EACCES;
  
  if(mode & W_OK != 0 && ((good_mode && 0222) & status.st_mode ==0))
    return -EACCES;

  if(mode & X_OK != 0 && ((good_mode && 0111) & status.st_mode ==0))
    return -EACCES;

  return 0;
}

static int FS_truncate(const char *path, off_t new_size)
{
  FS_Block *sb = FS_PRIVATE_DATA->super_block;
  FS_Block *file_head = FS_getblock(sb,sb[FS_SuperBlock_root_block]), *main_inode;
  off_t old_size;
  int needed_blocks;
  file_head = FS_resolve(file_head,path,sb);
  if(file_head == NULL)
    return -ENOENT;

  switch(file_head[FS_INode_magic_number])
    {
    case MAGIC_DIR:
      return -EISDIR;
    case MAGIC_DATA:
      break;
    case MAGIC_FREE_INODE:default:
      return -EIO;
    }

  old_size = file_head[FS_INode_size];
  log_msg("FS_truncate (%s, old size: %d)\n",path,file_head[FS_INode_size]);
  
  if(old_size < new_size)
    {
      needed_blocks = (int)ceil(1.0*new_size/FS_BLOCK_SIZE);
      log_msg("Enlarging file. Need %d blocks. ",needed_blocks);
      needed_blocks -= (int)ceil(old_size/FS_BLOCK_SIZE);
      log_msg("Enlarging file. Have %d blocks.\n",(int)ceil(old_size/FS_BLOCK_SIZE));
      FS_Unit pointer = 0;
      if(needed_blocks > sb[FS_SuperBlock_num_free_blocks])
	{
	  error_log("Not enough blocks. Need: %d Have: %d",needed_blocks,sb[FS_SuperBlock_num_free_blocks]);
	  return -ENOSPC;
	}
      while(pointer < FS_INODE_NUM_POINTERS)
	if(file_head[FS_INode_pointers + pointer] == 0)
	  break;
	else
	  {
	    pointer++;
	    file_head[FS_INode_size]++;
	  }
      
      main_inode = file_head;
      while(needed_blocks > 0)
	{
	  if(pointer == FS_INODE_NUM_POINTERS)
	    {
	      file_head[FS_INode_size] = pointer*FS_BLOCK_SIZE;
	      file_head[FS_INode_indirect] = FS_malloc(sb);
	      if(file_head[FS_INode_indirect] == 0)
		{
		  error_log("Ran out of space when allocating an indirect inode\n");
		  return -ENOSPC;
		}
	      log_msg("Allocated an indirect node at %x\n",file_head[FS_INode_indirect]);
	      file_head = FS_getblock(sb,file_head[FS_INode_indirect]);
	      FS_mkinode(file_head,sb[FS_SuperBlock_block_size]);
	      pointer = 0;
	    }
	  file_head[FS_INode_pointers + pointer++] = FS_malloc(sb);
	  log_msg("Saving %d to %d\n",file_head[FS_INode_pointers + pointer - 1],pointer - 1);
	  needed_blocks--;
	}
      file_head[FS_INode_size] = new_size % FS_BLOCK_SIZE;
      return 0;
    }

  //In this case we need to shrink the file.
  log_msg("Shrinking file\n");
  error_log("FINISH shrinking in truncate!!!\n");
  return -ENOSPC;
#if 0
  needed_blocks = (int)ceil(new_size/FS_BLOCK_SIZE);
  for(;needed_blocks < FS_INODE_NUM_POINTERS;needed_blocks++)
    {
      if(file_head[FS_INode_pointers + needed_blocks] == 0)
	continue;
      FS_free_block(sb, file_head[FS_INode_pointers + needed_blocks]);
      file_head[FS_INode_pointers + needed_blocks] = 0;
      }
  
  file_head[FS_INode_size] = new_size;
  return 0;
#endif  
}

int fs_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset,
	       struct fuse_file_info *fi)
{

  const struct fs_fuse_state *the_state = FS_PRIVATE_DATA;
  log_msg("FS_readdir (%s)\n",path);
  FS_Block *the_dir = FS_getblock(the_state->super_block,the_state->super_block[FS_SuperBlock_root_block]);
  size_t dir_counter = 0,pointer_counter = FS_INode_pointers,dirent_len = 0;
  unsigned char *d_name = NULL, *dirent;
  
  (void)offset;
  (void)fi;
  if(strcmp(path,"/") != 0)
    the_dir = FS_resolve(the_dir,path,the_state->super_block);
  else
    {
      filler(buf,"dump",NULL,0);
      filler(buf,"eeprom",NULL,0);
      filler(buf,"picc",NULL,0);
    }

  if(the_state == NULL)
    return -ENOENT;
  
  filler(buf, ".", NULL, 0);
  filler(buf, "..", NULL, 0);

  dir_counter = 0;
  for(;dir_counter < the_dir[FS_INode_size];dir_counter++)
    {
      size_t len;
      if(dirent_len % FS_BLOCK_SIZE == 0)
	dirent = FS_getblock(the_state->super_block,the_dir[pointer_counter++]);
      len = (size_t)dirent[0];
      if(len == 0)
	{
	  dirent = FS_getblock(the_state->super_block,the_dir[pointer_counter++]);
	  len = (size_t)dirent[0];
	}
      d_name = (unsigned char*)realloc(d_name,(len+1)*sizeof(unsigned char));
      dirent += 1;
      memcpy(d_name,dirent,len);
      d_name[len] = 0;
      filler(buf,d_name,NULL,0);
      dirent += len + 1;
      dirent_len += len + 2;
      log_msg("dirlen = %d\n",dirent_len);
    }
  free(d_name);
  return 0;
}

static int FS_opendir(const char *path, struct fuse_file_info *fi)
{
  const struct fs_fuse_state *the_state = FS_PRIVATE_DATA;
  FS_Block *the_dir = the_state->super_block;
  the_dir += the_state->super_block[FS_SuperBlock_root_block];

  log_msg("FS_opendir (%s)\n",path);

  if(strcmp(path,"/") != 0)
    the_dir = FS_resolve(the_dir,path,the_state->super_block);
  
  if(the_dir == NULL)
    return ENOENT;
  
    return 0;

}

static int FS_mkfile(const char *path, mode_t mode)
{
  FS_Block *sb = FS_PRIVATE_DATA->super_block;
  FS_Block *file = FS_resolve(FS_getblock(sb,sb[FS_SuperBlock_root_block]),path,sb);
  FS_Unit pointer = 0;
  const char *delim = NULL;
  char filename[FS_BLOCK_SIZE-2];
  memset(filename,0,sizeof(char)*(FS_BLOCK_SIZE-2));
  
  log_msg("FS_mkfile (%s) mode 0%d\n",path,mode & 0777);
  
  if(file != NULL)
    {
      error_log("File %s already exists\n",path);
      return -EEXIST;
    }
  
  if(sb[FS_SuperBlock_num_free_blocks] < 1)
    {
      error_log("No more free blocks\n",path);
      return -ENOSPC;
    }
  
  // does sub directory exist?
  delim = strrchr(path,'/');
  if(delim != NULL && delim != path)
    {
      char *subdir = calloc(strlen(path)+1,sizeof(char));
      memcpy(subdir,path,(delim - path)*sizeof(char));
      memcpy(filename,delim+1,sizeof(char)*strlen(delim+1));
      file = FS_resolve(FS_getblock(sb,sb[FS_SuperBlock_root_block]),subdir,sb);
      if(file == NULL)
	{
	  error_log("Must first create the subdirectory %s\n",subdir);
	  free(subdir);
	  return -ENOENT;
	}
      else if(file[FS_INode_magic_number] != MAGIC_DIR)
	{
	  error_log("%s is not a directory\n",subdir);
	  return -ENOTDIR;
	}
      free(subdir);
    }
  else
    {
      file = FS_getblock(sb,sb[FS_SuperBlock_root_block]);
      strncpy(filename,path+1,(FS_BLOCK_SIZE-2));
    }

  //Does the directory have an directory listings (i.e. is it empty)
  if(file[FS_INode_pointers] == 0)
    {
      FS_Block *dir_list;
      if(sb[FS_SuperBlock_num_free_blocks] < 2)
	{
	  error_log("Could not get a free inode for this directory\n");
	  return -ENOSPC;
	}
      file[FS_INode_pointers] = sb[FS_SuperBlock_free_queue];
      dir_list = FS_getblock(sb,sb[FS_SuperBlock_free_queue]);
      sb[FS_SuperBlock_free_queue] = dir_list[FS_INode_pointers];
      sb[FS_SuperBlock_num_free_blocks]--;
      memset(dir_list,0,FS_BLOCK_SIZE*sizeof(FS_Unit));
    }
  
  for(;pointer < FS_INODE_NUM_POINTERS;pointer++)
    {
      FS_Unit *dir_list;
      FS_Unit size = FS_BLOCK_SIZE, count = 0;
      if(file[FS_INode_pointers + pointer] == 0)
	{
	  file[FS_INode_pointers + pointer] = sb[FS_SuperBlock_free_queue];
	  dir_list = FS_getblock(sb,file[FS_INode_pointers + pointer]);
	  sb[FS_SuperBlock_free_queue] = dir_list[FS_INode_pointers];
	  sb[FS_SuperBlock_num_free_blocks]--;
	  memset(dir_list,0,FS_BLOCK_SIZE*sizeof(FS_Unit));
	}
      else
	dir_list = FS_getblock(sb,file[FS_INode_pointers + pointer]);

      while(count < FS_BLOCK_SIZE)
	{
	  if(*dir_list == 0)
	    break;
	  size -= *dir_list + 2;
	  count += *dir_list + 2;
	  dir_list += *dir_list + 2;
	}
      if(size >= strlen(filename))
	{
	  *dir_list = strlen(filename);
	  dir_list++;
	  memcpy(dir_list,filename,strlen(filename)*sizeof(char));
	  dir_list += strlen(filename);
	  *dir_list = sb[FS_SuperBlock_free_queue];
	  file[FS_INode_size]++;
	  file = FS_getblock(sb,*dir_list);
	  sb[FS_SuperBlock_free_queue] = file[FS_INode_pointers];
	  sb[FS_SuperBlock_num_free_blocks]--;
	  FS_mkinode(file,FS_BLOCK_SIZE);
	  log_msg("created %s\n", filename);
	  return 0;
	}
    }

  error_log("Could not get a free inode for this directory\n");
  return -ENOSPC;
  
}

static int FS_mknod(const char *path, mode_t mode, dev_t dev)
{
  log_msg("FS_mknod (%s)\n",path);
  return FS_mkfile(path,mode);
}

static int FS_open(const char *path, struct fuse_file_info *fi)
{
  FS_Block *sb = FS_PRIVATE_DATA->super_block;
  FS_Block *file = FS_resolve(FS_getblock(sb,sb[FS_SuperBlock_root_block]),path,sb);
  size_t len;

  log_msg("FS_open (%s, 0x%x)\n",path,file);

  if(FS_is_virtual(path) && strcmp(path,"/") != 0)
    return 0;
  if(file == NULL)
    return -ENOENT;
  return 0;
}

static int FS_removefile(const char *path, FS_Block *sb)
{
  FS_Block *top_dir = FS_getblock(sb,sb[FS_SuperBlock_root_block]);
  FS_Block *inode = FS_resolve(top_dir,path,sb);
  FS_Block *curr_block = NULL;
  if(FS_is_virtual(path))
    return -EACCES;
  
  if(inode == NULL)
    return -ENOENT;
  
  if(inode[FS_INode_magic_number] == MAGIC_DIR && inode[FS_INode_size] != 0)
    return -EACCES;
      
  inode[FS_INode_size] = 0;
  
  //attempt to remove entry in directory
  if(strchr(path,'/') != NULL)
    {
      size_t len = strlen(path) - strlen(strrchr(path,'/'))+2;
      char parent[len];
      int have_dir_name = FALSE;
      FS_Block *dirlist = NULL;
      
      memset(parent,0,len);
      strncpy(parent,path,len-1);
      inode = FS_resolve(top_dir,parent,sb);
      if(inode != NULL)
	{
	 FS_Unit dir_ent = 0;
	  for(;dir_ent < inode[FS_INode_size];dir_ent++)
	    {
	      size_t index = 0;
	      FS_Block *byte_start;
	      dirlist = FS_getblock(sb, inode[FS_INode_pointers + dir_ent]);
	      byte_start = dirlist;
	      for(;index < FS_BLOCK_SIZE;index++)
		{
		  size_t len = (size_t)dirlist[0];dirlist++;
		  if(strncmp(dirlist,path + strlen(parent),len) == 0)
		    {
		      FS_Block tmp_filelist[FS_BLOCK_SIZE];
		      FS_Block *data_inode = FS_getblock(sb,dirlist[len]);
		      log_msg("Freeing contents of inode %x\n",dirlist[len]);
		      //Free data inodes
		      if(data_inode != NULL)
			{
			  FS_Unit *free_inode = data_inode + FS_INode_pointers;
			  while(*free_inode != 0)
			    {
			      FS_free_block(sb,*free_inode);
			      free_inode++;
			    }
			  FS_free_block(sb,dirlist[len]);
			}
		      
		      have_dir_name = TRUE;
		      dirlist--;
		      //shift directory listing
		      memset(tmp_filelist,0,FS_BLOCK_SIZE);
		      memcpy(tmp_filelist,dirlist + len + 2,FS_BLOCK_SIZE - (dirlist - byte_start) - len - 2);
		      memcpy(dirlist,tmp_filelist,FS_BLOCK_SIZE - (dirlist - byte_start));
		      inode[FS_INode_size]--;
		      if(byte_start[0] == 0)
			{
			  // if directory listing is now empty, remove it from the inode and shift
			  FS_free_block(sb,inode[FS_INode_pointers + dir_ent]);
			  inode[FS_INode_pointers + dir_ent] = 0;
			}
		      break;
		    }
		  dirlist += len + 1;		      
		}
	      if(have_dir_name)
		break;
	    }
	}
    }
  

  return 0;
  
}

static int FS_unlink(const char *path)
{
  FS_Block *sb = FS_PRIVATE_DATA->super_block;
  FS_Block *file_head = FS_getblock(sb,sb[FS_SuperBlock_root_block]);
  log_msg("FS_unlink (%s)\n",path);
  if(FS_is_virtual(path))
    return -EACCES;
  file_head = FS_resolve(file_head,path,sb);
  if(file_head == NULL)
    return -ENOENT;
  if((file_head[FS_INode_mode] & 02 == 0))
    return -EACCES;
  return FS_removefile(path, FS_PRIVATE_DATA->super_block);
}

static int FS_write(const char *path, char *buf, size_t size, off_t offset,
                      struct fuse_file_info *fi)
{
  FS_Block *sb = FS_PRIVATE_DATA->super_block;
  FS_Block *file_head = FS_getblock(sb,sb[FS_SuperBlock_root_block]);
  FS_Unit data_ptr = 0;
  FS_Block *data_block = NULL;
  size_t amount_written = 0;
  file_head = FS_resolve(file_head,path,sb);
  
  log_msg("FS_write (file = %s, text = %s, size = %d, offset = %d)\n",path,buf,size,offset);
  
  if(file_head == NULL)
    {
      int retval = FS_mkfile(path, 075);
      if(retval != 0)
	return retval;
      file_head = FS_resolve(file_head,path,sb);
    }
  if(file_head == NULL)
    return 0;
  if(file_head[FS_INode_size] != offset + size)
    {
      int retval;
      log_msg("expanding to write. new size: %d\n",offset+size);
      retval = FS_truncate(path,offset + size);
      if(retval != 0)
	return retval;
    }
  
  if(offset >= 0)
    {
      data_ptr = (FS_Unit)floor(offset/FS_BLOCK_SIZE);
      if(data_ptr > FS_INODE_NUM_POINTERS)
	{
	  error_log("Offset exceeds inode space.\n");
	  return -EFAULT;
	}
    }
  
  while(amount_written != size)
    {
      if(data_ptr >= FS_INODE_NUM_POINTERS)
	{
	  if(file_head[FS_INode_indirect] == 0)
	    break;
	  file_head = FS_getblock(sb,file_head[FS_INode_indirect]);
	  data_ptr = 0;
	}
      data_block = FS_getblock(sb,file_head[FS_INode_pointers + data_ptr]);
      if(size > FS_BLOCK_SIZE)
	{
	  memcpy(data_block,buf,FS_BLOCK_SIZE*sizeof(char));
	  amount_written += FS_BLOCK_SIZE;
	  buf += FS_BLOCK_SIZE;
	  size -= FS_BLOCK_SIZE;
	}
      else
	{
	  memcpy(data_block,buf,size*sizeof(char));
	  amount_written += size;
	  break;
	}
	       
      data_ptr++;
    }

  return amount_written;
}


static int FS_read(const char *path, char *buf, size_t size, off_t offset,
		   struct fuse_file_info *fi)
{
  int len;
  int retval = 0;
  (void) fi;

  FS_Block *sb = FS_PRIVATE_DATA->super_block;
  FS_Block *file_head = sb;
  FS_Block *file = NULL;
  FS_Unit data_ptr = 0;
  log_msg("FS_read (%s)\n",path);
  if(strcmp(path,"/dump") == 0 )
    {
      len = (2+sb[FS_SuperBlock_num_blocks])*sb[FS_SuperBlock_block_size];
      log_msg("Filesystem length: %d Requesting: %d\n",len,size);
      if(size+offset > len)
	size = len-offset;
      memcpy(buf,&sb[offset], size);
      log_msg("Read %d bytes of the raw file system.\n",size);
      return size;
    }
  else if(strcmp(path,"/eeprom") == 0)
    {
      FILE *eeprom_file = tmpfile();
      len = FS_read_eeprom(sb,eeprom_file);
      if(offset != 0)
	fseek(eeprom_file,offset,SEEK_SET);
      log_msg("eeprom length = %d\n",len);
      size = fread(buf,sizeof(char),len,eeprom_file);
      fclose(eeprom_file);
      return len;
    }
  else if(strcmp(path,"/picc") == 0)
    {
      FILE *eeprom_file = tmpfile();
      len = FS_read_picc(sb,eeprom_file);
      if(offset != 0)
	fseek(eeprom_file,offset,SEEK_SET);
      log_msg("picc length = %d\n",len);
      size = fread(buf,sizeof(char),len,eeprom_file);
      fclose(eeprom_file);
      return len;
    }
  else
    {
      file = FS_resolve(FS_getblock(sb,sb[FS_SuperBlock_root_block]),path,sb);
      if(file == NULL)
	return -ENOENT;
      len = (size_t)FS_file_size(file);
      log_msg("%s is %d bytes long starting at 0x%x\n",path,len,file[FS_INode_pointers]);
    }
  if (offset < len) {
    if (offset + size > len)
      size = len - offset;
    data_ptr = 0;
    while(size > 0)
      {
	int write_amount = (size > FS_BLOCK_SIZE) ? FS_BLOCK_SIZE : size;
	file_head = FS_getblock(sb, file[FS_INode_pointers + data_ptr]);
	write_amount -= offset;
	offset = 0;
	log_msg("Copying %d bytes from 0x%x to 0x%x\n",write_amount,file_head,buf);
	log_msg("Reading block %d\n",file[FS_INode_pointers+ data_ptr]);
	memcpy(buf,file_head, write_amount);
	buf += write_amount;
	data_ptr++;
	if(data_ptr >= FS_INODE_NUM_POINTERS)
	  {
	    file = FS_getblock(sb,file[FS_INode_indirect]);
	    data_ptr = 0;
	  }

	retval += write_amount;
	size -= write_amount;
      }
    
  } else
    retval = 0;

  log_msg("Read %d bytes of %s\n",size,path);

  return retval;
}

static int FS_chmod(char *path, mode_t mode)
{
  FS_Block *inode = NULL;
  FS_Block *sb = FS_PRIVATE_DATA->super_block;

  log_msg("FS_chmod (%s)\n",path);
  if(FS_is_virtual(path))
    return -EACCES;
  
  inode = FS_getblock(sb, sb[FS_SuperBlock_root_block]);
  inode = FS_resolve(inode,path,sb);
  if(inode == NULL)
    return -ENOENT;
  
  /*if(inode[FS_INode_uid] != getuid())
    return -EACCES;*/
  
  inode[FS_INode_mode] = ((mode & 0700) >> 3) | (mode & 07);
  
  return 0;
}

static FS_Block* FS_format(struct fs_fuse_state *the_state)
{
  FS_Block *super_block = NULL, *rootdir = NULL, *data = NULL;
  FS_Block *testdir = NULL;
  int block_count = 2;
  int num_blocks = the_state->num_blocks, block_size = the_state->block_size;
  FS_allocate(&super_block,num_blocks,the_state->block_size);
  FS_mksuperblock(super_block,the_state);
  
  //setup top most inode
  rootdir = &super_block[block_size*super_block[FS_SuperBlock_root_block]];
  FS_mkinode(rootdir, the_state->block_size);

  //make a directory
  rootdir[FS_INode_magic_number] = MAGIC_DIR;
  rootdir[FS_INode_size]++;
  rootdir[FS_INode_pointers] = block_count++;
  data = &super_block[block_size*rootdir[FS_INode_pointers]];
  data[0] = strlen("README");
  memcpy(data + 1,"README",(size_t)data[0]);
  data = data +((off_t) 1+ data[0]);
  data[0] = block_count;data++;
  testdir = &super_block[block_size*block_count];
  FS_mkinode(testdir, the_state->block_size);
  testdir[FS_INode_uid] = 123;
  testdir[FS_INode_pointers] = ++block_count;
  strcat(&super_block[block_size*block_count++],"Formatted PICFS\n");
  sprintf(&super_block[block_count*block_size],"%d Blocks\n", super_block[FS_SuperBlock_num_blocks]);testdir[FS_INode_pointers + 1] = block_count++;
  sprintf(&super_block[block_size*block_count],"%d byte blocks\n", super_block[FS_SuperBlock_block_size]);testdir[FS_INode_pointers + 2] = block_count++;
  testdir[FS_INode_size] = 1+strlen("%d byte blocks\n");
  
  super_block[FS_SuperBlock_num_free_blocks] = super_block[FS_SuperBlock_num_blocks] - block_count;

  super_block[FS_SuperBlock_free_queue] = block_count;

  while(block_count < super_block[FS_SuperBlock_num_blocks])
    {
      FS_Block *freed = &super_block[block_size*block_count++];
      freed[FS_SuperBlock_magic_number] = MAGIC_FREE_INODE;
      freed[FS_INode_pointers] = block_count;
    }
 
  return super_block;
}

static FS_Block* FS_mount(const char *filename, struct fs_fuse_state *the_state)
{
  FS_Block *super_block = NULL;
  FILE *dev = NULL;
  size_t len = 0;
  if(access(filename,R_OK) != 0)
      return NULL;
  
  dev = fopen(filename,"r");
  if(dev == NULL)
    return NULL;
  
  fseek(dev,0,SEEK_END);
  len = ftell(dev);
  rewind(dev);
  
  if(len % the_state->block_size != 0)
    {
      error_log("File system %s has an incomplete block.\n\tSize: %d\n\tBlock size: %d\n",filename,len,the_state->block_size);
      return NULL;
    }

  super_block = (FS_Unit*)malloc(len);
  if(fread(super_block,1,len,dev) != len)
    {
      error_log("Could not read all of %s\n",filename);
      free(super_block);
      return NULL;
    }

  fclose(dev);
  return super_block;
}

static int FS_flush(const char *buf,struct fuse_file_info *info)
{
  log_msg("FS_flush (%s)\n",buf);
  return 0;
}

/*static int FS_create(const char *buf,mode_t mode, struct fuse_file_info *info)
{
  log_msg("FS_create (%s)\n",buf);
  return 0;
  }*/

static int FS_release(const char *path, struct fuse_file_info *fi)
{
  log_msg("FS_release (%s)\n",path);
  return 0;
}

static int FS_utime(const char *path, const struct timespec *times)
{
  log_msg("FS_utime (%s)\n",path);
  return 0;
}

static int FS_fsync(const char *path, int thing, struct fuse_file_info *fi)
{
  log_msg("FS_fsync (%s)\n",path);
  return 0;
}

static int FS_statfs(const char *path, struct statvfs *statv)
{
  FS_Block *sb = FS_PRIVATE_DATA->super_block;
  statv->f_bsize = sb[FS_SuperBlock_block_size];
  statv->f_frsize = sb[FS_SuperBlock_block_size];
  statv->f_blocks = sb[FS_SuperBlock_num_blocks];
  statv->f_bfree = sb[FS_SuperBlock_num_free_blocks];
  statv->f_bavail = statv->f_bfree;//super block + first inode
  statv->f_files = (FS_getblock(sb,sb[FS_SuperBlock_root_block]))[FS_INode_size];
  statv->f_ffree = sb[FS_SuperBlock_num_free_blocks];
  statv->f_favail = statv->f_ffree;
  statv->f_namemax = statv->f_bsize - 2;
  
  return 0;
}

struct fuse_operations fs_ops = {
  .access = FS_access,
  .readdir = fs_readdir,
  .getattr = fs_getattr,
  .opendir = FS_opendir,
  .mknod = FS_mknod,
  .flush = FS_flush,
  .write = FS_write,
  .open = FS_open,
  .read = FS_read,
  .release = FS_release,
  .statfs = FS_statfs,
  .truncate = FS_truncate,
  .unlink = FS_unlink,
  .chmod = FS_chmod,
  /*.create = FS_create,*/
  .utime = FS_utime,
  .fsync = FS_fsync
};

static const struct option long_opts[] = {
  {"block_size",1,NULL,'b'},
  {"log",1,NULL,'l'},
  {"mount",1,NULL,'m'},
  {"num_blocks",1,NULL,'n'},
  {"verbose",0,NULL,'v'},
  {"help",0,NULL,'h'},
  {0,0,0,0}
};
static const char short_opts[] = "b:hl:m:n:v";

static void print_help()
{
  const struct option *opts = long_opts;
  printf("fs -- PIC file system mount tool, using libfuse.\n");
  printf("Copyright 2011 David Coss, PhD\n");
  printf("-------------------------------\n");
  printf("Mounts a file system on a given mount point.\n");
  printf("If no image file is specified, a fresh filesystem is used.\n");
  printf("\n");
  printf("Usage: ./fs [options] <mount point>\n");
  printf("Options:\n");
  
  while(opts->name != NULL)
    {
      printf("--%s",opts->name);
      if(opts->val >= 'a' && opts->val <= 'z')
	printf(", -%c ",opts->val);
      else
	printf("      ");
      if(opts->has_arg)
	printf("<ARG>  ");
      else
	printf("       ");
     
      switch(opts->val)
	{
	case 'b':
	  printf("Sepcify the size of a block, in bytes. Default: 16 bytes");
	  break;
	case 'l':
	  printf("Specify a log file.");
	  break;
	case 'm':
	  printf("Mount a specific filesystem image.");
	  break;
	case 'n':
	  printf("Specify the number of blocks to be created. Default: 16");
	case 'h':
	  printf("This message.");
	  break;
	case 'v':
	  printf("Verbose. Without this flag, only errors are written to the log. Default: disabled");
	  break;
	default:
	  break;
	}
      printf("\n");
      opts++;
    }
}

static void FS_parse_args(struct fs_fuse_state *the_state, int argc, char **argv)
{
  char ch;
  while((ch = getopt_long(argc,argv,short_opts,long_opts,NULL)) != -1)
    {
      switch(ch)
	{
	case 'b':
	  {
	    int tempint;
	    if(sscanf(optarg,"%d",&tempint) != 1)
	    {
	      fprintf(stderr,"Could not read block size: %s\n",optarg);
	      exit(-1);
	    }
	    if(tempint > 0xff)
	      {
		printf("Block size cannot exceed 255 bytes due to size of address variables.\n");
		exit( EFAULT);
	      }
	    the_state->block_size = (FS_Unit)(tempint & 0xff);
	    break;	
	  }
	case 'l':
	  the_state->logfile = fopen(optarg,"w");
	  if(the_state->logfile == NULL)
	    {
	      fprintf(stderr,"Could not open: %s\n",optarg);
	      fprintf(stderr,"Reason: %s\n",strerror(errno));
	      exit(errno);
	    }
	  break;
	case 'h':
	  print_help();
	  exit(0);
	case 'm':
	  the_state->super_block = FS_mount(optarg,the_state);
	  if(the_state->super_block == NULL)
	    {
	      fprintf(stderr,"Could not mount: %s\n",optarg);
	      exit(-1);
	    }
	  else
	    {
	      fprintf(stderr,"Mounted %s\n",optarg);
	    }
	  break;
	case 'n':
	  {
	    int tempint;
	    if(sscanf(optarg,"%d",&tempint) != 1)
	      {
		fprintf(stderr,"Could not read specified number of blocks: %s\n",optarg);
		exit(-1);
	      }
	    if(tempint > 0xff)
	      {
		printf("Cannot use more than 255 blocks due to size of address variables.\n");
		exit( EFAULT);
	      }
	    the_state->num_blocks = (FS_Unit)(tempint & 0xff);
	    break;
	  }
	case 'v':
	  the_state->verbose_log = TRUE;
	  break;
	default:
	  fprintf(stderr,"Unknown flag: %c\n",ch);
	  exit(-1);
	}
    }
}

void FS_default_state(struct fs_fuse_state *the_state)
{
  if(the_state == NULL)
    return;
  the_state->super_block = NULL;
  the_state->logfile = stderr;
  the_state->rootdir = NULL;
  the_state->verbose_log = FALSE;
  the_state->num_blocks = 16;
  the_state->block_size = 16;
}

FILE *assembly_file;
int main(int argc, char **argv)
{
  int fuse_stat;
  struct fs_fuse_state *the_state;
  FS_Block *super_block = NULL;
  assembly_file = tmpfile();
  the_state = calloc(1,sizeof(struct fs_fuse_state));
  //defaults
  FS_default_state(the_state);
  FS_parse_args(the_state,argc,argv);
  setvbuf(the_state->logfile, NULL, _IOLBF, 0);
  super_block = the_state->super_block;

  //setup super block
  if(super_block == NULL)
    super_block = FS_format(the_state);

  //setup FUSE
  if(super_block == NULL)
    {
      error_log("Could not format/read super block.");
      return -1;
    }
  the_state->super_block = super_block;
  
  #if 1
  // Initialize an empty argument list
  struct fuse_args args = FUSE_ARGS_INIT(0, NULL);
  if(optind < argc)
    {
      fuse_opt_add_arg(&args,argv[0]);
      fuse_opt_add_arg(&args,argv[optind]);
      fprintf(the_state->logfile,"Mount point = %s\n",args.argv[args.argc-1]);
    }
  else
    {
      fprintf(the_state->logfile,"No mount point given.\n");
      exit(-1);
    }
  fuse_stat = fuse_main(args.argc,args.argv, &fs_ops, the_state);
#else
  fuse_stat = 0;
  FILE *dump = fopen("debug","w");
  FS_removefile("/cat",super_block);
  fwrite(super_block,sizeof(FS_Unit),super_block[FS_SuperBlock_num_blocks] * super_block[FS_SuperBlock_block_size],dump);
  fclose(dump);
#endif
  time_t starttime = time(NULL);
  if(fuse_stat == 0)
    fprintf(the_state->logfile,"fuse mounted %s on %s\n",args.argv[args.argc-1],ctime(&starttime));
  return fuse_stat;
}




