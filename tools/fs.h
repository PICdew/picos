#ifndef FS_H
#define FS_H

#define FUSE_USE_VERSION 26
#include <stdio.h>

typedef unsigned char FS_Unit;
typedef unsigned char FS_Block;


#ifdef NOT_FOR_PIC
struct fs_fuse_state {
  FILE *logfile;
  int verbose_log;
  char *rootdir;
  FS_Unit *super_block;
  FS_Unit num_blocks,block_size;
};
#define FS_PRIVATE_DATA ((struct fs_fuse_state*)fuse_get_context()->private_data)
#define FS_BLOCK_SIZE (((struct fs_fuse_state*)fuse_get_context()->private_data)->block_size)


#endif//not for pic

enum FS_MAGIC_NUMBERS{ 
  MAGIC_SUPERBLOCK = 0,
  MAGIC_DIR,
  MAGIC_DATA,
  MAGIC_FREE_INODE
};

#define FS_REVISION_NUM 1

enum{
  FS_INode_magic_number = 0,
  FS_INode_uid,
  FS_INode_mode,
  FS_INode_size,
  FS_INode_indirect,
  FS_INode_pointers,
  FS_INode_length
};
#define FS_INODE_NUM_POINTERS (FS_BLOCK_SIZE - FS_INode_pointers)

enum {
  FS_SuperBlock_magic_number = 0,
  FS_SuperBlock_magic_number2,
  FS_SuperBlock_magic_number3,
  FS_SuperBlock_magic_number4,
  FS_SuperBlock_revision_num,
  FS_SuperBlock_block_size,
  FS_SuperBlock_num_blocks,
  FS_SuperBlock_num_free_blocks,
  FS_SuperBlock_free_queue,
  FS_SuperBlock_root_block,
  FS_SuperBlock_length
};

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif

#endif// FS_H

