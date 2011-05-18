#include <htc.h>
#include <string.h>

#include "defines.h"
#include "arg.h"
#include "lcd.h"
#include "usart.h"
#include "io.h"
#include "utils.h"

void IO_putd(char d)
{
  char hex_val[2];
  hex_to_word(&hex_val,d);
  putch(hex_val[0]);
  putch(hex_val[1]);
}

void IO_puts(const char *str)
{
  if(str == NULL)
    return;
  while(*str != 0)
    {
      putch(*str);
      str++;
    }
}

char get_button_state()
{
  char retval = ((button_port & 0b111000) >> button_phase);
  __delay_ms(button_debounce_time);
  retval &= ((button_port & 0b111000) >> button_phase);
  return ~retval & 7;
}

void morse_ditdat_sound(char dat_not_dit)
{
  if(SOUND_counter != 0)
    return;// can only set, not update.
  SOUND_counter = 4;
  if(dat_not_dit == TRUE)
    SOUND_counter = SOUND_counter * 2;
  tone_440();
}


const unsigned char morse_encoding[] =
  {
    0b01000010,    
    0b10000100,  // b
    0b10100100,  // c
    0b10000011,  // d
    0b00000001,  // e
    0b00100100,  // f
    0b11000011,  // g
    0b00000100,  // h
    0b00000010,  // i
    0b01110100,  // j
    0b10100011,  // k
    0b01000100,  // l
    0b11000010,  // m
    0b10000010,  // n
    0b11100011,  // o
    0b01100100,  // p
    0b11010100,  // q
    0b01000011,  // r
    0b00000011,  // s
    0b10000001,  // t
    0b00100011,  // u
    0b00010100,  // v
    0b01100011,  // w
    0b10010100,  // x
    0b10110100,  // y
    0b11000100,  // z
    0b11111101,  // 0
    0b01111101,  // 1
    0b00111101,  // 2
    0b00011101,  // 3
    0b00001101,  // 4
    0b00000101,  // 5
    0b10000101,  // 6
    0b11000101,  // 7
    0b11100101,  // 8
    0b11110101  // 9
  };



void morse_ditdat_sound_blocking(char encoded)
{
  char count = (encoded & 7);
  char i = 0;
  for(;i < count;i++)
    {
      tone_440();
      __delay_ms(96);
      if(encoded & 0x80)
	__delay_ms(192);
      mute_sound();
      __delay_ms(48);
      encoded = encoded << 1;
    }
}

void morse_sound(const char *str)
{
  while(*str != 0)
    {
      if(*str >= 0x41 && *str <= 0x5A)
	morse_ditdat_sound_blocking(morse_encoding[*str - 0x41]);
      else if(*str >= 0x61 && *str <= 0x7A)
	morse_ditdat_sound_blocking(morse_encoding[*str - 0x61]);
      else if(*str >= 0x30 && *str <= 0x39)
	morse_ditdat_sound_blocking(morse_encoding[*str - 0x17]);
      str++;
    }
}


char get_command()
{
  char button_val;
  char ditdat = 0;
  char command_hash = 0;
  static bit have_command;
  char char_counter = 0;
  clear_output();
  ARG_clear();
  IO_puts("$");
  have_command = FALSE;

  while(TRUE)
    {
      if(indev == IN_USART || indev == IN_USART_BTNS)
	{
	  char received_char[2];
	  USART_timeout = 100;
	  ditdat = usart_getch();
	  received_char[0] = ditdat;received_char[1] = 0;
	  morse_sound(received_char);
	}
      if((indev == IN_USART_BTNS && ditdat == 0) || indev == IN_BTNS)
	{
	  char button_val = '@';
	  button_val = get_button_state();
	  while((button_val & BTN_RTN) == 0)
	    {
	      if((button_val & BTN_DIT) != 0)
		{
		  ditdat = (ditdat << 1) + 1;
		  morse_ditdat_sound(FALSE);
		  __delay_ms(400);
		}
	      else if((button_val & BTN_DAT) != 0)
		{
		  ditdat = (ditdat+1) << 1;
		  morse_ditdat_sound(TRUE);
		  __delay_ms(400);
		}
	      button_val = get_button_state();
	      if((button_val & BTN_RTN) != 0)
		{
		  ditdat = morse_to_char(ditdat);
		  __delay_ms(400);
		}
	    }
	}
      if(ditdat == 0)
	continue;
		
      if(ditdat == 0xa || ditdat == 0xd)
	break;
      ARG_putch(ditdat);
      char_counter++;
      if(ditdat == ' ')
	{
	  have_command = TRUE;
	  ARG_next = char_counter;
	}
      if(!have_command)
	{
	  calculate_crc(&command_hash,ditdat);
	}
      clear_output();
      IO_puts("?");
      IO_puts(ARG_buffer);
      ditdat = 0;
    }
  ditdat = ARG_getch();
  if(ditdat == 0 || ditdat == ' ')
    ARG_next = ARG_SIZE;
  return command_hash;
}

char morse_to_char(char morse)
{
  static const char morse_table[] = "?etianmsurwdkgohvf\x7Fl\xapjbxcyzq??54?3???2??+????16=/?????7???8?90";
  if(morse >= strlen(morse_table))
    return '?';
  return morse_table[morse];
}


void tone_440()
{
  PR2 = 0x8d ;
  CCPR1L = 0;
  TRISC5 = 0;
  T2CON = 0b00000111 ;
  CCP1CON = 0x3c;
  CCPR1L = 0x46;
}

void mute_sound()
{
  CCP1CON = 0;
}

