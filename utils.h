
extern void hex_to_word(char *two_chars, char hex);
extern void calculate_crc(char *crc, char newval);


// CRC command hashes
#define CRC_time 0xAA
#define CRC_usart 0xd9
#define CRC_morse 0xe2
