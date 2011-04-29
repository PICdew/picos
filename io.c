#include <htc.h>
#include <stdio.h>
#include <string.h>

#include "defines.h"

#include "lcd.h"
#include "usart.h"
#include "io.h"

void putch(char c)
{ 
  switch(outdev)
    {
    case OUT_LCD_USART:
      lcd_putch(c);
    case OUT_USART:
      usart_putch(c);
      break;
    case OUT_LCD:default:
      lcd_putch(c); 
      break;
    }
} 

void clear_output()
{
  switch(outdev)
    {
    case OUT_LCD:case OUT_LCD_USART:
      lcd_clear();
      break;
    case OUT_USART:
      break;
    }
}

char get_button_state()
{
  char retval = button_port >> button_phase;
  __delay_ms(button_debounce_time);
  retval &= button_port >> button_phase;
  return retval;
}

void calculate_crc(char *crc, char newval)
{
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
}

void morse_ditdat_sound(char dat_not_dit)
{
  SOUND_counter = 12;
  if(dat_not_dit)
    SOUND_counter = SOUND_counter * 3;
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
      __delay_us(48);
      if(encoded & 0x80)
	__delay_us(96);
      mute_sound();
      __delay_us(24);
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
  clear_output();
  printf("$");
  while(TRUE)
    {
      if(indev == IN_USART || indev == IN_USART_BTNS)
	{
	  USART_timeout = 100;
	  ditdat = usart_getch();
	}
      if((indev == IN_USART_BTNS && ditdat == 0) || indev == IN_BTNS)
	{
	  button_val = get_button_state();
	  if(button_val == 0)
	    continue;
	  if(button_val & BTN_DIT)
	    {
	      ditdat = (ditdat << 1) + 1;
	      morse_ditdat_sound(0);
	    }
	  else if(button_val & BTN_DAT)
	    {
	      ditdat = (ditdat+1) << 1;
	      morse_ditdat_sound(1);
	    }
	  else if(button_val & BTN_RTN == 0)
	    continue;
	}
      if(ditdat == 0)
	continue;
      ditdat = morse_to_char(ditdat);
      if(ditdat != 0xa || ditdat != 0xd)
	break;
      printf("%c",ditdat);
      calculate_crc(&command_hash,ditdat);
    }
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
  TRISC5 = 1;
  PR2 = 0b10001101 ;
  T2CON = 0b00000111 ;
  CCPR1L = 0b01000110 ;
  CCP1CON = 0b00111100 ;
}

void mute_sound()
{
  CCP1CON = 0;
}

