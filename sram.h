/**
 * PICOS, PIC operating system.
 * Author: David Coss, PhD
 * Date: 1 Sept 2011
 * License: GNU Public License version 3.0 (see http://www.gnu.org)
 *
 */
#ifndef SRAM_H
#define SRAM_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

extern void SRAM_write(unsigned int addr, const void *buffer, char n);
extern void SRAM_read(unsigned int addr, void *buffer, char n);

#endif // SRAM_H

