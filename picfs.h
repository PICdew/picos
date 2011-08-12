#ifndef PICFS_H
#define PICFS_H

#include "fs.h"

#define PICFS_FILENAME_MAX (FS_BUFFER_SIZE-2)

typedef char file_t;// file handle type
typedef unsigned int offset_t;

enum {PICFS_SET, PICFS_CURR, PICFS_END,PICFS_REVERSE};

#define ST_SIZE 0 // size is 2 bytes, big endian

char picfs_pwd[4];// working directory

signed char picfs_mount(const char *addr);
signed char picfs_open(const char *name);
signed char picfs_close(file_t fh);
signed char picfs_read(file_t fh);
signed char picfs_seek(file_t fh, offset_t offset, char whence);
offset_t picfs_tell(file_t fh);
void picfs_rewind(file_t fh);
char picfs_is_open(file_t fh); 

#endif
