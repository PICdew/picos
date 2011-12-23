#ifndef SRAM_H
#define SRAM_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

extern void SRAM_write(unsigned int addr, const void *buffer, char n);
extern void SRAM_read(unsigned int addr, void *buffer, char n);

#endif // SRAM_H

