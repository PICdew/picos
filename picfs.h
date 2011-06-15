#include "fs.h"

#define PICFS_FILENAME_MAX (FS_BUFFER_SIZE-2)

typedef char file_t;// file handle type

signed char picfs_open(const char *name);
char picfs_close(file_t fh);
char picfs_read(file_t fh);

char picfs_is_open(file_t fh); 
