#ifndef NOT_FOR_PIC
#include <htc.h>
#endif

extern void hex_to_word(char *two_chars, char hex);
extern void calculate_crc(char *crc, char newval);

#define DelayuS(x) _delay((unsigned long)(x))
#define DelaymS(x) _delay((unsigned long)((x)*(1000L)))


// CRC command hashes
#define CRC_echo 0x1
#define CRC_time 0x15
#define CRC_date 0x14
#define CRC_usart 0x61
#define CRC_morse 0x66
#define CRC_run 0x69
#define CRC_usart2eeprom 0x5
#define CRC_eeprom2usart 0x12
