#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>
#include "fs.h"

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
#include <sys/types.h>
#include <sys/xattr.h>

static int FS_is_virtual(const char *path)
{
  return (strcmp(path,"/") == 0) || (strcmp(path,"/dump") == 0);
}

FS_Block* FS_getblock(FS_Block *super_block, FS_Unit block_id)
{
  return &(super_block[block_id*FS_BLOCK_SIZE]);
}

void FS_mksuperblock(FS_Block *block, size_t num_blocks)
{
  if(block == NULL)
    return;

  block[FS_SuperBlock_magic_number] = MAGIC_SUPERBLOCK;
  block[FS_SuperBlock_revision_num] = FS_REVISION_NUM;
  block[FS_SuperBlock_block_size] = FS_BLOCK_SIZE;
  block[FS_SuperBlock_num_blocks] = (FS_Unit)ceil((double)num_blocks/FS_BLOCK_SIZE);
  block[FS_SuperBlock_num_free_blocks] = block[FS_SuperBlock_num_blocks] - 1;
  block[FS_SuperBlock_root_block] = 1;

  block[FS_SuperBlock_free_queue] = 2;
}

void FS_mkinode(FS_Block *inode)
{
  size_t i;
  if(inode == NULL)
    return;

  inode[FS_INode_magic_number] = MAGIC_DATA;
  inode[FS_INode_uid] = 0;
  inode[FS_INode_mode] = 075;
  inode[FS_INode_size] = 0;
  
  i = 0;
  for(;i<FS_INODE_NUM_POINTERS;i++)
    inode[FS_INode_pointers + i] = 0;
}

static void FS_error(const char *str)
{
  fprintf(FS_PRIVATE_DATA->logfile,"FS Error: %s\n",str);
}

FILE* error_file;

int FS_allocate(FS_Block **data,size_t num_blocks)
{
  if(data == NULL)
    return -1;

  *data = (FS_Block*)realloc(*data,num_blocks*FS_BLOCK_SIZE*sizeof(FS_Unit));
  return 0;
}

static void FS_inode2stat(struct stat *stbuf, const FS_Block *the_dir)
{
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
      stbuf->st_size = the_dir[FS_INode_size];
    }
    
  stbuf->st_nlink = 1;
}


static FS_Block* FS_resolve(FS_Block *dir, char *path, FS_Block *sb)
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
	      if(strcmp(d_name,token) == 0)
		{
		  dir = sb + dirent[0];
		  moved_up = TRUE;
		  if(dir[FS_INode_magic_number] != MAGIC_DIR)
		    {
		      free(rw_path);
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
  return dir;
}



static int fs_getattr(const char *path, struct stat *stbuf)
{
    int res = 0;
    const FS_Block *sb = FS_PRIVATE_DATA->super_block;
    const FS_Block *dir = sb + sb[FS_SuperBlock_root_block];

    if(strcmp(path,"/") == 0)
      FS_inode2stat(stbuf,dir);

    if(strcmp("/dump",path) == 0)
      {
	stbuf->st_mode = 0555 | S_IFREG;
	stbuf->st_size = sb[FS_SuperBlock_num_blocks] * sb[FS_SuperBlock_block_size];
	return 0;
      }

    dir = FS_resolve(dir,path,sb);
    if(dir == NULL)
      return -ENOENT;
    
    FS_inode2stat(stbuf,dir);
    
    return res;
}

int fs_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset,
	       struct fuse_file_info *fi)
{

  const struct fs_fuse_state *the_state = FS_PRIVATE_DATA;
  FS_Block *the_dir = FS_getblock(the_state->super_block,the_state->super_block[FS_SuperBlock_root_block]);
  size_t dir_counter = 0;
  unsigned char *d_name = NULL, *dirent;
  
  (void)offset;
  (void)fi;

  if(strcmp(path,"/") != 0)
    the_dir = FS_resolve(the_dir,path,the_state->super_block);
  else
    {
      filler(buf,"dump",NULL,0);
    }

  if(the_state == NULL)
    return -ENOENT;
  
  filler(buf, ".", NULL, 0);
  filler(buf, "..", NULL, 0);

  dir_counter = 0;
  dirent = FS_getblock(the_state->super_block,the_dir[FS_INode_pointers]);
  for(;dir_counter < the_dir[FS_INode_size];dir_counter++)
    {
      size_t len = (size_t)dirent[0];
      d_name = (unsigned char*)realloc(d_name,(len+1)*sizeof(unsigned char));
      dirent += 1;
      memcpy(d_name,dirent,len);
      d_name[len] = 0;
      filler(buf,d_name,NULL,0);
      dirent += len + 1;
    }
  free(d_name);
  return 0;
}

static int FS_opendir(const char *path, struct fuse_file_info *fi)
{
  const struct fs_fuse_state *the_state = FS_PRIVATE_DATA;
  FS_Block *the_dir = the_state->super_block;
  the_dir += the_state->super_block[FS_SuperBlock_root_block];

  if(strcmp(path,"/") != 0)
    the_dir = FS_resolve(the_dir,path,the_state->super_block);
  
  if(the_dir == NULL)
    return ENOENT;
  
    return 0;

}

static int FS_open(const char *path, struct fuse_file_info *fi)
{
  FS_Block *sb = FS_PRIVATE_DATA->super_block;
  FS_Block *file = FS_resolve(sb+sb[FS_SuperBlock_root_block],path,sb);
  size_t len;
  
  if(strcmp(path,"/dump") == 0)
    return 0;
  if(file == NULL)
    return -ENOENT;
  return 0;
}

  static int FS_removefile(const char *path, FS_Block *sb)
{
  FS_Block *inode = FS_resolve(sb+sb[FS_SuperBlock_root_block],path,sb);
  FS_Block *curr_block = NULL;
  if(FS_is_virtual(path))
    return -EACCES;
  
  if(inode == NULL)
    return -ENOENT;
  
  if(inode[FS_INode_magic_number] == MAGIC_DIR && inode[FS_INode_size] != 0)
    return -EACCES;
      
  sb[FS_SuperBlock_num_free_blocks] += (FS_Unit)ceil((double)inode[FS_INode_size]/FS_BLOCK_SIZE);
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
      inode = FS_resolve(sb+sb[FS_SuperBlock_root_block],parent,sb);
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
			  memset(tmp_filelist,0,FS_BLOCK_SIZE);
			  memcpy(tmp_filelist,inode + ((off_t)FS_INode_pointers + dir_ent + 1) , FS_INODE_NUM_POINTERS - dir_ent - 1 );
			  memcpy(inode + FS_INode_pointers + dir_ent,tmp_filelist,FS_INODE_NUM_POINTERS - dir_ent);
			  sb[FS_SuperBlock_num_free_blocks]++;
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
  return FS_removefile(path, FS_PRIVATE_DATA->super_block);
}


static int FS_read(const char *path, char *buf, size_t size, off_t offset,
                      struct fuse_file_info *fi)
{
    size_t len;
    int retval = 0;
    (void) fi;

    FS_Block *sb = FS_PRIVATE_DATA->super_block;
    FS_Block *file_head = sb;
    FS_Block *file = NULL;
    static const FS_Block testit[] = "this is a test";
    if(strcmp(path,"/dump") == 0)
      {
	len = sb[FS_SuperBlock_num_blocks]*sb[FS_SuperBlock_block_size];
      }
    else
      {
	file = FS_resolve(FS_getblock(sb,sb[FS_SuperBlock_root_block]),path,sb);
	if(file == NULL)
	  return -ENOENT;
	len = (size_t)file[FS_INode_size];
	file_head = FS_getblock(sb, file[FS_INode_pointers]);
      }
    if (offset < len) {
        if (offset + size > len)
            size = len - offset;
        memcpy(buf, file_head + offset, size);
    } else
        size = 0;
    return size;
}

static int FS_chmod(const char *path, mode_t mode)
{
  FS_Block *inode = NULL;
  FS_Block *sb = FS_PRIVATE_DATA->super_block;
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

static FS_Block* FS_format(size_t num_blocks)
{
  FS_Block *super_block = NULL, *rootdir = NULL, *data = NULL;
  FS_Block *testdir = NULL;
  int block_count = 2;
  FS_allocate(&super_block,num_blocks);
  FS_mksuperblock(super_block,num_blocks);
  
  //setup top most inode
  rootdir = FS_getblock(super_block,super_block[FS_SuperBlock_root_block]);
  FS_mkinode(rootdir);

  //make a directory
  rootdir[FS_INode_magic_number] = MAGIC_DIR;
  rootdir[FS_INode_size]++;
  rootdir[FS_INode_pointers] = block_count++;
  data = FS_getblock(super_block, rootdir[FS_INode_pointers]);
  data[0] = strlen("testfile");
  memcpy(data + 1,"testfile",(size_t)data[0]);
  data = data +((off_t) 1+ data[0]);
  data[0] = block_count;data++;
  data[0] = strlen("cat");data++;
  memcpy(data,"cat",3);data += 3;
  testdir = FS_getblock(super_block, block_count);
  FS_mkinode(testdir);
  testdir[FS_INode_uid] = 123;
  testdir[FS_INode_pointers] = block_count++;
  strcat(FS_getblock(super_block, block_count),"Hello, World!\n");
  testdir[FS_INode_size] = 1+strlen(FS_getblock(super_block,block_count));
  
  data[0] = block_count++;
  testdir = FS_getblock(super_block, block_count++);
  FS_mkinode(testdir);
  testdir[FS_INode_uid] = 123;
  testdir[FS_INode_pointers] = block_count++;
  strcat(FS_getblock(super_block, testdir[FS_INode_pointers]),"Meow!\n");
  testdir[FS_INode_size] = 1+strlen(FS_getblock(super_block, testdir[FS_INode_pointers]));
  rootdir[FS_INode_size]++;

  super_block[FS_SuperBlock_num_free_blocks] -= block_count -1;

  super_block[FS_SuperBlock_free_queue] = block_count;

  block_count++;
  while(block_count >= 0)
    {
      FS_Block *freed = FS_getblock(super_block,super_block[FS_SuperBlock_free_queue] + (block_count--));
      freed[FS_SuperBlock_magic_number] = MAGIC_FREE_INODE;
      freed[FS_INode_pointers] = super_block[FS_SuperBlock_free_queue] + (block_count+2);
    }
 
  return super_block;
}

static FS_Block* FS_mount(const char *filename)
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
  
  if(len % FS_BLOCK_SIZE != 0)
    {
      printf("File system %s has an incomplete block.\n\tSize: %d\n\tBlock size: %d\n",filename,len,FS_BLOCK_SIZE);
      return NULL;
    }

  super_block = (FS_Unit*)malloc(len);
  if(fread(super_block,1,len,dev) != len)
    {
      printf("Could not read all of %s\n",filename);
      free(super_block);
      return NULL;
    }

  fclose(dev);
  return super_block;
}

struct fuse_operations fs_ops = {
  .readdir = fs_readdir,
  .getattr = fs_getattr,
  .opendir = FS_opendir,
  .open = FS_open,
  .read = FS_read,
  .unlink = FS_unlink,
  .chmod = FS_chmod
};

enum {FS_FLAG_LOG=0};
static const struct option long_opts[] = {
  {"log",1,NULL,FS_FLAG_LOG},
  {"load",1,NULL,'l'},
  {"help",0,NULL,'h'}
};
static const char short_opts[] = "hl:";//log is long opt only

static void FS_parse_args(struct fs_fuse_state *the_state, int argc, char **argv)
{
  char ch;
  while((ch = getopt_long(argc,argv,short_opts,long_opts,NULL)) != -1)
    {
      switch(ch)
	{
	case FS_FLAG_LOG:
	  the_state->logfile = fopen(optarg,"w+");
	  if(the_state->logfile == NULL)
	    {
	      fprintf(stderr,"Could not open: %s\n",optarg);
	      fprintf(stderr,"Reason: %s\n",strerror(errno));
	      exit(errno);
	    }
	  break;
	case 'h':
	  printf("Help not yet added");
	  exit(0);
	case 'l':
	  the_state->super_block = FS_mount(optarg);
	  if(the_state->super_block == NULL)
	    {
	      fprintf(stderr,"Could not mount: %s\n",optarg);
	      exit(-1);
	    }
	  else
	    {
	      printf("Mounted %s\n",optarg);
	    }
	  break;
	default:
	  fprintf(stderr,"Unknown flag: %c\n",ch);
	  exit(-1);
	}
    }
}


int main(int argc, char **argv)
{
  int fuse_stat;
  struct fs_fuse_state the_state;
  FS_Block *super_block = NULL;
  size_t num_blocks = 256;
  
  //defaults
  the_state.super_block = NULL;
  the_state.logfile = stderr;
  the_state.rootdir = NULL;
  FS_parse_args(&the_state,argc,argv);
  super_block = the_state.super_block;

  //setup super block
  if(super_block == NULL)
    super_block = FS_format(num_blocks);

  //setup FUSE
  if(super_block == NULL)
    {
      FS_error("Could not format/read super block.");
      return -1;
    }
  the_state.super_block = super_block;
  
  #if 1
  // Initialize an empty argument list
  struct fuse_args args = FUSE_ARGS_INIT(0, NULL);
  if(optind < argc)
    {
      fuse_opt_add_arg(&args,argv[0]);
      fuse_opt_add_arg(&args,argv[optind]);
      printf("Mount point = %s\n",args.argv[args.argc-1]);
    }
  else
    {
      printf("No mount point given.\n");
      exit(-1);
    }
  fuse_stat = fuse_main(args.argc,args.argv, &fs_ops, &the_state);
#else
  fuse_stat = 0;
  FILE *dump = fopen("debug","w");
  FS_removefile("/cat",super_block);
  fwrite(super_block,sizeof(FS_Unit),super_block[FS_SuperBlock_num_blocks] * super_block[FS_SuperBlock_block_size],dump);
  fclose(dump);
#endif
  printf("fuse_main returned %d\n",fuse_stat);
  return fuse_stat;
}



