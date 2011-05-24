enum FS_MAGIC_NUMBERS{ MAGIC_SUPERBLOCK = 0,MAGIC_DIR,MAGIC_DATA };
#define FS_REVISION_NUM 0
#define FS_BLOCK_SIZE 16

#define FS_INODE_NUM_POINTERS 3
typedef struct {
  unsigned char magic_number;
  unsigned char uid;
  unsigned char mode;
  unsigned char size;
  unsigned char pointers[FS_INODE_NUM_POINTERS];  
}FS_INode;

typedef struct{
  unsigned char data[FS_BLOCK_SIZE];
}FS_Block;

typedef struct {
  unsigned char magic_number;
  unsigned char revision_num;
  unsigned char block_size;//size
  unsigned char num_free_inodes;
  FS_INode *root_block;
}FS_SuperBlock;


