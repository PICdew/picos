
#ifndef button_debounce_time 
#define button_debounce_time 10 //ms
#endif

enum{OUT_LCD,OUT_USART,OUT_LCD_USART};
enum{IN_USART,IN_BTNS,IN_USART_BTNS};

enum{IO_CMD_LCD=1,IO_CMD_LCD_CLEAR,IO_CMD_SD_READ,IO_CMD_LCD_TIME};
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
extern void morse_ditdat_sound(char dat_not_dit);

void IO_puts(const char *str);
void IO_putd(char d);
extern void IO_flush();
