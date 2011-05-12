#include <htc.h>
#include <stdio.h>
#include "lcd.h"

#define	LCD_RS RA0
#define	LCD_RW RA1
#define LCD_EN RA2
#define LCD_DATA	PORTC

#define	LCD_STROBE()	((LCD_EN = 1),(LCD_EN=0))

void lcd_write(char c)
{
	if(lcd_pos == LCD_WIDTH && LCD_RS)
	{
	  LCD_RS = 0;
	  __delay_us(40);
	  LCD_DATA = ( 0xC);
	  LCD_STROBE();
	  LCD_DATA = ( 0);
	  LCD_STROBE();
	  LCD_RS = 1;
	  lcd_pos++;
	}
	else if(lcd_pos == LCD_EOF  && LCD_RS)
	{
	  LCD_RS = 0;
		__delay_us(40);
	  LCD_DATA = (8);
	  LCD_STROBE();
	  LCD_DATA = ( 0);
	  LCD_STROBE();
	  lcd_pos = 0;
	  LCD_RS = 1;
	}
	else if(LCD_RS)
		lcd_pos++;
  __delay_us(40);
  LCD_DATA = ( ( c >> 4 ) & 0x0F );
  LCD_STROBE();
  LCD_DATA = ( c & 0x0F );
  LCD_STROBE();
}


void lcd_clear(void)
{
	LCD_RS = 0;
	lcd_write(0x1);
	__delay_ms(2);
	lcd_pos = 0;
}

void lcd_puts(const char * s)
{
	LCD_RS = 1;	
	while(*s)
	  {
	    if(*s == '\n' && lcd_pos <= LCD_WIDTH)
	      lcd_goto(0x40);
	    else if(*s == '\n')
	      lcd_goto(0);
	    lcd_write(*s++);
	  }
}

void lcd_putch(char c)
{
  if(c == '\n')
    if(lcd_pos <= LCD_WIDTH)
      {
	lcd_goto(0x40);
	return;
      }
    else
      {
	lcd_goto(0);
	return;
      }
  
  LCD_RS = 1;
  lcd_write( c );
}

void lcd_goto(unsigned char pos)
{
	LCD_RS = 0;
	lcd_write(0x80+pos);
	if(pos < 8)
	  lcd_pos = pos;
	else if(pos >= 0x40 && pos <= (0x40+ LCD_WIDTH))
	  lcd_pos = pos - 40;
	else
	  lcd_pos = 0;
}
	
void lcd_init()
{
	char init_value;

	ANSEL = 0;
	ANSELH = 0;

	init_value = 0x3;
	LCD_RS = 0;
	LCD_EN = 0;
	LCD_RW = 0;
	
	__delay_ms(15);
	LCD_DATA	 = init_value;
	LCD_STROBE();
	__delay_ms(5);
	LCD_STROBE();
	__delay_us(200);
	LCD_STROBE();
	__delay_us(200);
	LCD_DATA = 2;
	LCD_STROBE();

	lcd_write(0x28); // Set interface length
	lcd_write(0xF); // Display on, Cursor on, Cursor Blink
	lcd_clear();	// Clear screen
	lcd_write(0x6); // Set entry Mode
	lcd_pos = 0; // should be done by clear, but just to be safe set it explicitly...
}

