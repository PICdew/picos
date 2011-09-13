/**
 * PICOS, PIC operating system.
 * Author: David Coss, PhD
 * Date: 1 Sept 2011
 * License: GNU Public License version 3.0 (see http://www.gnu.org)
 *
 * This file provides various utility functions.
 */
#include "utils.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stddef.h>
const picos_size_t PICLANG_magic_numbers[2] = {0x6f46,0x006f};

void hex_to_word(char *two_chars, char hex)
{
  if(two_chars == NULL)
    return;
  two_chars[0] = (hex & 0xf0) >> 4;
  two_chars[1] = hex & 0xf;
  
  for(hex = 0;hex < 2;hex++)
    {
      if(two_chars[hex] > 9)
	two_chars[hex] = two_chars[hex] + 0x37;
      else
	two_chars[hex] = two_chars[hex] + 0x30;
    }
}

#include <stdio.h>
void dec_to_word(char *five_chars, picos_size_t hex)
{
  picos_size_t mask = 1;
  char counter;
  if(five_chars == NULL)
    return;
  counter = 1;
  for(;counter < PICOS_SIZE_T_DECIMAL_DIGITS;counter++)
    mask *= 10;
  counter = 0;
  for(;counter < PICOS_SIZE_T_DECIMAL_DIGITS-1;counter++)
    {
      five_chars[counter] = hex/mask;
      hex -= five_chars[counter]*mask;
      five_chars[counter] += 0x30;
      mask /= 10;
    }
  five_chars[PICOS_SIZE_T_DECIMAL_DIGITS-1] = hex + 0x30;
}

void calculate_crc(char *crc, char newval)
{
#if 0//old
  *crc = *crc ^ newval;
  newval = 0x3b;
  if(*crc & 1)
    newval ^= 0x1d;
  if(*crc & 2)
    newval ^= 0x3a;
  if(*crc & 3)
    newval ^= 0x74;
  if(*crc & 4)
    newval ^= 0xe8;
  if(*crc & 5)
    newval ^= 0xcd;
  if(*crc & 6)
    newval ^= 0x87;
  if(*crc & 7)
    newval ^= 0x13;
  if(*crc & 8)
    newval ^= 0x26;
  *crc = newval;
#endif
  *crc ^= newval;
}

void picos_delay(unsigned long cycles)
{
  while(cycles != 0)
    cycles--;
}


