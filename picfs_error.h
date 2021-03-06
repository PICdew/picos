/**
 * PICOS, PIC operating system.
 * Author: David Coss, PhD
 * Date: 1 Sept 2011
 * License: GNU Public License version 3.0 (see http://www.gnu.org)
 *
 * This file lists error codes used in PICOS
 */
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
  PICFS_ENOMEM,
  PICFS_NOENT,
  PICFS_ENFILE,
  PICFS_EBADF,
  PICFS_EINVAL,
  PICFS_EAGAIN,
  PICFS_BUSY,
  PICFS_TOO_MANY_MOUNTED /** mount table space is full **/,
  PICFS_INVALID_FILENAME,
  PICFS_INVALID_FILESYSTEM,
  PICFS_EOF,
  PICLANG_UNKNOWN_ERROR,
  PICLANG_NO_SUCH_PROGRAM,
  PICLANG_SUSPENDED,
  PICLANG_UNKNOWN_COMMAND,
  PICLANG_PC_OVERFLOW,
  PICLANG_SEGV,
  PICLANG_STACK_OVERFLOW,
  PICLANG_NULL_POINTER,
  PICLANG_INVALID_EXECUTABLE,
  PICLANG_INVALID_PARAMETER,
  THREAD_TOO_MANY_THREADS,
  THREAD_CANNOT_INTERRUPT,
  THREAD_INVALID_SIGNAL
};

typedef unsigned char picos_error_t;
volatile picos_error_t error_code;

/**
 * Sets 'error_code' to the provided value and returns -1.
 */
signed char error_return(char the_error);


#endif
