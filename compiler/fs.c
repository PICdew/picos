#include "fs.h"

void FS_mksuperblock(FS_SuperBlock *block)
{
  if(block == NULL)
    return;

  block->magic_number = MAGIC_SUPERBLOCK;
  block->revision_num = FS_REVISION_NUM;
  block->block_size = FS_BLOCK_SIZE;
  block->num_free_inodes = 1;
}

void FS_mkinode(FS_INode *inode)
{
  size_t i;
  if(inode == NULL)
    return;

  inode->magic_number = MAGIC_INODE;
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
  dest[i++] = source->num_free_inodes
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
  dest->num_free_inode = source[i++];
}

unsigned char* FS_inode2array(unsigned char *dest, const FS_INode *source)
{
  size_t i = 0;
  if(source == NULL)
    return NULL;
  if(dest == NULL)
    dest = (unsigned char*)malloc(sizeof(FS_INode)/sizeof(unsigned char));
  dest[i++] = source->magic_number;
  for(;i < sizeof(FS_INode);i++)
    dest[i] = source->pointers[i-1];
}

FS_SuperBlock* FS_array2inode(FS_INode *dest, const unsigned char *source)
{
  size_t i = 0;
  if(source == NULL)
    return NULL;
  if(dest == NULL)
    dest = (FS_INode*)malloc(sizeof(FS_INode));
  dest->magic_number = source[i++];
  for(;i < sizeof(FS_INode);i++)
    dest->pointers[i-1] = source[i];
}

void FS_init(unsigned char **data)
{
  FS_SuperBlock sblock;
  FS_INode inode;
  size_t i = 0;
  unsigned char *top_most_dir;
  if(data == NULL)
     return;
  *data = (unsigned char*)malloc(sizeof(unsigned char)*FS_BLOCK_SIZE);
  FS_mksuperblock(&sblock);
  FS_superblock2array(data,&sblock);
  
  top_most_dir = &data[sizeof(FS_SuperBlock)/sizeof(unsigned char)];
  
  FS_mkinode(&inode);
  
  
}
