
extern void hex_to_word(char *two_chars, char hex);
extern void calculate_crc(char *crc, char newval);


// CRC command hashes
#define CRC_time 0x15
#define CRC_date 0x14
#define CRC_usart 0x61
#define CRC_morse 0x66
#define CRC_run 0x69
#define CRC_usart2eeprom 0x5
#define CRC_eeprom2usart 0x12
