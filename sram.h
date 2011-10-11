#ifndef SRAM_H
#define SRAM_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

extern void SRAM_write(unsigned int addr, const void *buffer, char n);
extern void SRAM_read(unsigned int addr, void *buffer, char n);

// SRAM block assignments
// These are the addresses (unsigned int) for temporary storage.
#define SRAM_MTAB_ADDR 0
#define SRAM_PICFS_WRITE_SWAP_ADDR FS_BUFFER_SIZE
#define SRAM_PICFS_OPEN_SWAP_ADDR (2*FS_BUFFER_SIZE)
#define SRAM_PICFS_ARG_SWAP_ADDR (3*FS_BUFFER_SIZE)
#endif // SRAM_H

