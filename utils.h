/**
 * PICOS, PIC operating system.
 * Author: David Coss, PhD
 * Date: 1 Sept 2011
 * License: GNU Public License version 3.0 (see http://www.gnu.org or COPYING)
 *
 * This file prototypes utility functions used throughout the PICOS.
 */
#ifndef PICOS_UTILS
#define PICOS_UTILS

typedef unsigned short picos_size_t;// On the picc, this is little endian!!!
typedef signed short picos_signed_t;// On the picc, this is little endian!!!
#define PICOS_SIZE_T_DECIMAL_DIGITS 5

extern void dec_to_word(char *three_chars, picos_size_t hex);
extern void hex_to_word(char *two_chars, char hex);
extern void calculate_crc(char *crc, char newval);
extern char TEMPERATURE_read();
extern void picos_delay(unsigned long cycles);


// CRC command hashes
#define CRC_echo 0x1
#define CRC_time 0x15
#define CRC_date 0x14
#define CRC_usart 0x61
#define CRC_dec 0x62
#define CRC_morse 0x66
#define CRC_enc 0x68//encrypt RSA
#define CRC_run 0x69
#define CRC_usart2eeprom 0x5
#define CRC_eeprom2usart 0x12


#endif //PICOS_UTILS
