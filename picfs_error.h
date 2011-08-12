#ifndef PICFS_ERROR_H
#define PICFS_ERROR_H 1

enum ERROR_CODES{
  SUCCESS = 0,
  SD_INIT_ERROR,
  SD_BLOCK_ERROR,
  SD_WRITE_ERROR,
  SD_READ_ERROR,
  ARG_BUFFER_OVERFLOW,
  ARG_INVALID,
  PAGE_INSUFFICENT_MEMORY,
  PAGE_NO_SUCH_ADDRESS,
  PAGE_PERMISSION_DENIED,
  PICFS_NOENT,
  PICFS_ENFILE,
  PICFS_EBADF,
  PICFS_EINVAL,
  PICFS_TOO_MANY_MOUNTED /** mount table space is full **/,
  PICFS_INVALID_FILENAME
};

char error_code;

/**
 * Sets 'error_code' to the provided value and returns -1.
 */
signed char error_return(char the_error);


#endif
