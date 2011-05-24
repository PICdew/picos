#include <stdlib.h>
#include <string.h>
#include "fs.h"

#ifdef NOT_FOR_PIC

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

#endif


void FS_mksuperblock(FS_SuperBlock *block)
{
  if(block == NULL)
    return;

  block->magic_number = MAGIC_SUPERBLOCK;
  block->revision_num = FS_REVISION_NUM;
  block->block_size = FS_BLOCK_SIZE;
  block->num_free_inodes = 1;
  block->root_block = NULL;
}

void FS_mkinode(FS_INode *inode)
{
  size_t i;
  if(inode == NULL)
    return;

  inode->magic_number = MAGIC_DATA;
  inode->uid = 0;
#ifdef NOT_FOR_PIC
  fuse_get_context()->uid;
#endif
  inode->mode = 074;
  inode->size = 0;
  
  i = 0;
  for(;i<FS_INODE_NUM_POINTERS;i++)
    inode->pointers[i] = 0;
}

unsigned char* FS_superblock2array(unsigned char *dest, const FS_SuperBlock *source)
{
  size_t i = 0;
  if(source == NULL)
    return NULL;
  if(dest == NULL)
    dest = (unsigned char*)malloc(sizeof(FS_SuperBlock)/sizeof(unsigned char));
  dest[i++] = source->magic_number;
  dest[i++] = source->revision_num;
  dest[i++] = source->block_size;
  dest[i++] = source->num_free_inodes;

  return dest;
}

FS_SuperBlock* FS_array2superblock(FS_SuperBlock *dest, const unsigned char *source)
{
  size_t i = 0;
  if(source == NULL)
    return NULL;
  if(dest == NULL)
    dest = (FS_SuperBlock*)malloc(sizeof(FS_SuperBlock));
  dest->magic_number = source[i++];
  dest->revision_num = source[i++];
  dest->block_size = source[i++];
  dest->num_free_inodes = source[i++];

  return dest;
}

unsigned char* FS_inode2array(unsigned char *dest, const FS_INode *source)
{
  size_t i = 0;
  if(source == NULL)
    return NULL;
  if(dest == NULL)
    dest = (unsigned char*)malloc(sizeof(FS_INode)/sizeof(unsigned char));
  dest[i++] = source->magic_number;
  dest[i++] = source->uid;
  dest[i++] = source->size;
  
  for(;i < sizeof(FS_INode);i++)
    dest[i] = source->pointers[i-1];

  return dest;
}

FS_INode* FS_array2inode(FS_INode *dest, const unsigned char *source)
{
  size_t i = 0;
  if(source == NULL)
    return NULL;
  if(dest == NULL)
    dest = (FS_INode*)malloc(sizeof(FS_INode));
  dest->magic_number = source[i++];
  dest->uid = source[i++];
  dest->size = source[i++];

  for(;i < sizeof(FS_INode);i++)
    dest->pointers[i-1] = source[i];

  return dest;
}

void FS_init(unsigned char **data)
{
  FS_SuperBlock sblock;
  FS_INode inode;
  unsigned char *top_most_dir;
  if(data == NULL)
     return;
  *data = (unsigned char*)malloc(sizeof(unsigned char)*FS_BLOCK_SIZE);
  FS_mksuperblock(&sblock);
  FS_superblock2array(*data,&sblock);
  
  top_most_dir = &(*data)[sizeof(FS_SuperBlock)/sizeof(unsigned char)];
  
  FS_mkinode(&inode);
  
  
}

#ifdef NOT_FOR_PIC

#define FUSE_USE_VERSION 26


FILE* error_file;

struct fs_fuse_state {
  FILE *logfile;
  char *rootdir;
  FS_SuperBlock *super_block;
  unsigned char *data;
  size_t data_size;
};
#define FS_PRIVATE_DATA ((struct fs_fuse_state*)fuse_get_context()->private_data)

int FS_allocate(unsigned char **data,size_t size)
{
  if(data == NULL)
    return -1;

  *data = (unsigned char*)realloc(*data,size*sizeof(unsigned char));
  return 0;
}

static void FS_inode2stat(struct stat *stbuf, const FS_INode *the_dir)
{
  if(stbuf == NULL || the_dir == NULL)
    return;

  stbuf->st_mode = S_IFDIR | ((the_dir->mode & 070) << 3) | (the_dir->mode);
  stbuf->st_nlink = 1;
  
#error FINISH FS_inode2stat
}

static int fs_getattr(const char *path, struct stat *stbuf)
{
    int res = 0;
    const struct fs_fuse_state *the_state = FS_PRIVATE_DATA;
    const FS_INode *the_dir = the_state->super_block->root_block;
    memset(stbuf, 0, sizeof(struct stat));
    

    return res;
}


int fs_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset,
	       struct fuse_file_info *fi)
{

  const struct fs_fuse_state *the_state = FS_PRIVATE_DATA;
  const FS_INode *the_dir = the_state->super_block->root_block;
  size_t dir_counter = 0;
  
  (void)offset;
  (void)fi;
  
if(the_state == NULL)
    return -ENOENT;

  
  
  filler(buf, ".", NULL, 0);
  filler(buf, "..", NULL, 0);
  
  dir_counter = 0;
  for(;dir_counter<the_dir->size;dir_counter++)
    {
      const unsigned char *inode_ent =  &the_state->data[the_dir->pointers[dir_counter] ];
      size_t len = (size_t)inode_ent[0];
      char *d_name = (char*)calloc(len+1,sizeof(char));
      inode_ent += 1;
      memcpy(d_name,inode_ent,len);
      inode_ent += len;
      filler(buf,d_name,NULL,0);
      inode_ent += 1;
    }
  
  return 0;
}


struct fuse_operations fs_ops = {
  .readdir = fs_readdir,
  .getattr = fs_getattr
};

int main(int argc, char **argv)
{
  int fuse_stat;
  struct fs_fuse_state the_state;
  FS_INode *rootdir;

  if(argc == 1)
    return -1;
  
  the_state.data = NULL;
  the_state.logfile = stderr;
  the_state.rootdir = argv[argc-1];
  the_state.super_block = (FS_SuperBlock*)malloc(sizeof(FS_SuperBlock));
  FS_mksuperblock(the_state.super_block);
  the_state.super_block->root_block = (FS_INode*)malloc(sizeof(FS_INode));
  rootdir = the_state.super_block->root_block;
  FS_mkinode(rootdir);
  rootdir->magic_number = MAGIC_DIR;

  FS_allocate(&the_state.data,1 + strlen("testdir") + 1);
  the_state.data[0] = strlen("testdir");
  memcpy(the_state.data + 1,"testdir",(size_t)the_state.data[0]);
  the_state.data[ 1+ the_state.data[0] ] = 1;
  
  rootdir->size = 1;
  rootdir->pointers[0] = 0;

  fuse_stat = fuse_main(argc,argv, &fs_ops, &the_state);

  printf("fuse_main returned %d\n",fuse_stat);
  return fuse_stat;
}

#endif


