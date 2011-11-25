#ifndef SPI_H
#define SPI_H

enum {
  DEV_SLAVE=0,
  DEV_SRAM
};

// Status bits
#define SPI_CMD_ACK 1
#define SPI_SIZE_ACK 2
#define SPI_PARAM_ACK 4

typedef signed char spi_dev_t;

extern void spi_close(spi_dev_t device);
extern void spi_open(spi_dev_t device);
extern char spi_sendrecv(char send_byte);

#endif//SPI_H
