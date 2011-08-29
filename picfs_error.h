#ifndef PICFS_ERROR_H
#define PICFS_ERROR_H 1

enum ERROR_CODES{
  SUCCESS = 0,
  SD_INIT_ERROR,
  SD_BLOCK_ERROR,
  SD_WRITE_ERROR,
  SD_READ_ERROR,
  TIME_INVALID,
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
  PICFS_INVALID_FILENAME,
  PICFS_INVALID_FILESYSTEM,
  PICFS_EOF,
  PICLANG_UNKNOWN_ERROR,
  PICLANG_NO_SUCH_PROGRAM,
  PICLANG_SUSPENDED,
  PICLANG_UNKNOWN_COMMAND,
  PICLANG_PC_OVERFLOW,
  PICLANG_MEMORY_OVERFLOW,
  PICLANG_STACK_OVERFLOW,
  PICLANG_NULL_POINTER,
  PICLANG_INVALID_EXECUTABLE
};

char error_code;

/**
 * Sets 'error_code' to the provided value and returns -1.
 */
signed char error_return(char the_error);


#endif
