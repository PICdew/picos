/**
 * PICOS, PIC operating system.
 * Author: David Coss, PhD
 * Date: 1 Sept 2011
 * License: GNU Public License version 3.0 (see http://www.gnu.org)
 *
 * This file provides the declarations for abstract IO processes.
 * Functions getch and putch should be implemented in hardware drivers
 * outside of PICOS.
 */
#ifndef PICOS_IO
#define PICOS_IO 

#include "picos/utils.h"

#ifndef button_debounce_time 
#define button_debounce_time 10 //ms
#endif

#define IO_CMD_BREAK1 0xde
#define IO_CMD_BREAK2 0xad

extern char get_button_state();

#ifndef button_bits
char button_bits;
#define button_port button_bits
#define button_phase 3
#endif

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
void IO_putd(picos_size_t d);

/**
 * Move cursor to coordinates. How this happens is up to the hardware
 * driver programmer. Thus, it is extern.
 *
 * Note: The arguments are row and column, in that order.
 */
extern void IO_move(picos_size_t row, picos_size_t column);

/**
 * Places the x and y coordinates of the cursor in the provided
 * memory locations. How this happens is up to the hardware
 * driver programmer. Thus, it is externed.
 *
 * Note: to ignore a coordinate, provide NULL.
 */
extern void IO_getxy(picos_size_t *x, picos_size_t *y);
extern void IO_flush();

#define OUT_LCD 1
#define OUT_USART 2
#define OUT_SOUND 4

#define IN_USART 1
#define IN_BTNS 2

// Commands
enum{
  IO_CMD_IDLE= 0,
  IO_CMD_LCD,
  IO_CMD_LCD_CLEAR,
  IO_CMD_USART_PUTCH,
  IO_CMD_USART_GETCH,
  IO_CMD_FOPEN,
  IO_CMD_FREAD,
  IO_CMD_LCD_TIME,
  IO_CMD_RSA_E,
  IO_CMD_RSA_D,
};

// Parameters arguments
#define IO_NO_ARGS -1


#endif // PICOS_IO
