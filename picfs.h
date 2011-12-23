/**
 * PICOS, PIC operating system.
 * Author: David Coss, PhD
 * Date: 1 Sept 2011
 * License: GNU Public License version 3.0 (see http://www.gnu.org)
 *
 * This file is the header for the filesystem used on PIC devices.
 */
#ifndef PICFS_H
#define PICFS_H

#include "fs.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "arg.h"
#include "scheduler.h"

// In order from least to most significant byte, the 
// data in SRAM for the file handle is:
// file's root inode, most significant byte of 
// next get offset (inode not byte address), and least significant byte
// of next get offset
#define FILE_HANDLE_SIZE 3

// Maximum number of open files.
// This determines the size of the file table in memory (FTAB)
#ifndef MAX_OPEN_FILES
#define MAX_OPEN_FILES 8
#endif

// SRAM block assignments
// These are the addresses (unsigned int) for temporary storage.
#define SRAM_MTAB_ADDR 0 // First byte is number of devices mounted, After that is the mount table
#define SRAM_PICFS_WRITE_SWAP_ADDR FS_BUFFER_SIZE // needs one FS_BUFFER_SIZE
#define SRAM_PICFS_OPEN_SWAP_ADDR (2*FS_BUFFER_SIZE)// needs one FS_BUFFER_SIZE
#define SRAM_PICFS_ARG_SWAP_ADDR (3*FS_BUFFER_SIZE) // needs PICOS_MAX_PROCESSES * ARG_SIZE
#define SRAM_PICFS_FTAB_ADDR (SRAM_PICFS_ARG_SWAP_ADDR + PICOS_MAX_PROCESSES * ARG_SIZE)
#define SRAM_PICFS_FILE_ADDR (SRAM_PICFS_FTAB_ADDR + (MAX_OPEN_FILES*FILE_HANDLE_SIZE))// Beginning of location of files that are "cat"-ed to SRAM


#define PICFS_FILENAME_MAX (FS_BUFFER_SIZE-2)

typedef char file_t;// file handle type
typedef unsigned int offset_t;

enum {PICFS_SET, PICFS_CURR, PICFS_END,PICFS_REVERSE};

#define ST_SIZE 0 // size is 2 bytes, big endian

#define SDCARD_ADDR_SIZE 4// Number of bytes in a SD card address (big endian)
char picfs_pwd[SDCARD_ADDR_SIZE];// working directory
volatile FS_Unit picfs_buffer[FS_BUFFER_SIZE];

signed char picfs_mount(const char *addr);
signed char picfs_chdir(char mount_point);
signed char picfs_open(const char *name);
signed char picfs_close(file_t fh);
signed char picfs_write(file_t fh);// file handle currently ignore and the raw file of the fs is used.
signed char picfs_read(file_t fh);
signed char picfs_seek(file_t fh, offset_t offset, char whence);
signed char picfs_stat(file_t fh);
offset_t picfs_tell(file_t fh);
void picfs_rewind(file_t fh);
char picfs_is_open(file_t fh); 
/**
 * Reads a file specified by "filename" and, depending on the
 * value of fileptr, writes it to:
 * -2: filesystem raw file
 * -1: the standard output (default)
 * >= 0: to a file * in SRAM specified by the index of the
 * first byte of the file in SRAM.
 */
void cat_file(const char *filename, int fileptr);

#endif
