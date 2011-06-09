#include "fs.h"

#define PICFS_FILENAME_MAX (FS_BUFFER_SIZE-2)

char picfs_buffer_block(FS_Unit block_id);
signed char picfs_open(const char *name);
char picfs_close(char fh);
char picfs_read(char fh);
char picfs_rewind(char fh);

char picfs_is_open(char fh); 
