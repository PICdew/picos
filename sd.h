
#include "utils.h"

/**
 * Blocking SPI write/read.
 *
 * Sends byte, waits for received flag, then returns received byte.
 */
extern char SPI_sendrecv(char data);

/**
 * Restarts the SD card. Sets the block size to "block_size" bytes
 */
extern void SD_restart(char block_size);

/**
 * Write a null terminated buffer.
 * the size of loc must be four bytes and loc must be big endian
 */
extern void SD_write(picos_addr_t address, void *buff, char n);

/**
 * Read n characters from SD
 * the size of addr must be four bytes and should be big endian
 */
extern void SD_read(picos_addr_t address, void *buff, char n);

/**
 * Sets the proper TRIS bits. Initializes master mode with clock idle high and sped = Fosc/4
 */
extern void SD_init(void);

/**
 * Restarts the SD card communication
 */
extern void SD_restart(char block_size);
