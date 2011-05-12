#define SD_CS RC6

/**
 * Blocking SPI write/read.
 *
 * Sends byte, waits for received flag, then returns received byte.
 */
char SPI_sendrecv(char data);

/**
 * Restarts the SD card. Sets the block size to "block_size" bytes
 */
void SD_restart(char block_size);

/**
 * Write a null terminated buffer.
 */
void SD_write(char *buff, char *loc);

/**
 * Read n characters from SD
 */
void SD_read(char *addr, char *buffer, char n);

/**
 * Sets the proper TRIS bits. Initializes master mode with clock idle high and sped = Fosc/4
 */
void SD_init(void);
