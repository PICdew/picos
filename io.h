#ifndef button_debounce_time 
#define button_debounce_time 10 //ms
#endif

#define IO_CMD_BREAK1 0xde
#define IO_CMD_BREAK2 0xad

extern char get_button_state();

typedef char out_t;
typedef char in_t;
out_t outdev;
in_t indev;
volatile char SOUND_counter;

extern char morse_to_char(char morse);
extern char get_command();

extern void clear_output();
extern void putch(char);

extern void mute_sound();
extern void tone_440();
void morse_sound(const char *str);
void morse_ditdat_sound(char dat_not_dit);
void morse_ditdat_sound_blocking(char encoded);
char morse_decode(char encode);

void IO_puts(const char *str);
void IO_putd(char d);
extern void IO_flush();

extern void SRAM_write(unsigned int addr, const char *buffer, char n);
extern void SRAM_read(unsigned int addr, char *buffer, char n);

#define OUT_LCD 1
#define OUT_USART 2
#define OUT_SOUND 4

#define IN_USART 1
#define IN_BTNS 2

enum{
  IO_CMD_LCD=1,
  IO_CMD_LCD_CLEAR,
  IO_CMD_FOPEN,
  IO_CMD_FREAD,
  IO_CMD_LCD_TIME,
  IO_CMD_RSA_E,
  IO_CMD_RSA_D
};
