#ifndef button_bits
#define button_bits PORTAbits
#endif
#ifndef button_port
#define button_port PORTA
#endif
#ifndef button_phase
#define button_phase 3
#endif
#ifndef button_debounce_time 
#define button_debounce_time 10 //ms
#endif

enum{OUT_LCD,OUT_USART,OUT_LCD_USART};
enum{IN_USART,IN_BTNS,IN_USART_BTNS};

enum{BTN1=0,BTN2,BTN3};
#define BTN_RTN 4 // masks for &'ing button bits
#define BTN_DIT 1
#define BTN_DAT 2

extern char get_button_state();

typedef char out_t;
typedef char in_t;
out_t outdev;
in_t indev;
volatile char SOUND_counter;

extern char morse_to_char(char morse);
extern char get_command();

extern void clear_output();

extern void mute_sound();
extern void tone_440();
void morse_sound(const char *str);
extern void morse_ditdat_sound(char dat_not_dit);

void IO_puts(const char *str);
void IO_putd(char d);
