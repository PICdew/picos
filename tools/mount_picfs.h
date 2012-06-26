/**
 * PICOS, PIC operating system.
 * Author: David Coss, PhD
 * Date: 1 Sept 2011
 * License: GNU Public License version 3.0 (see http://www.gnu.org)
 *
 * libfuse code for creating and mounting PICFS images
 */

#ifndef FS_H
#define FS_H

#include <stdio.h>

typedef unsigned char FS_Unit;
typedef unsigned char FS_Block;

enum FS_MAGIC_NUMBERS{ 
  MAGIC_SUPERBLOCK = 0,
  MAGIC_DIR,
  MAGIC_DATA,
  MAGIC_FREE_INODE,
  MAGIC_RAW
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
  FS_SuperBlock_raw_file/* Inode for the beginning of a raw file. */,
  FS_SuperBlock_root_block,
  FS_SuperBlock_length
};

#endif// FS_H

