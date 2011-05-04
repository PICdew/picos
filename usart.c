#include <htc.h>
#include <stdio.h>
#include "usart.h"

void usart_putch(unsigned char byte) 
{
  while(!TXIF) 
    continue;
  TXREG = byte;
}

char usart_getch() 
{
  while(!USART_have_incoming())	/* set when register is not empty */
    continue;
  return RCREG;	
}

char usart_getch_block() 
{
  while(!RCIF)	/* set when register is not empty */
    continue;
  return RCREG;	
}

char usart_getche(void)
{
  unsigned char c;
  putch(c = getch());
  return c;
}

void usart_init()
{
  //RX_PIN = 1;	
  //TX_PIN = 1;		  
  ANSEL = 0;
  ANSELH = 0;
  SPBRG = DIVIDER;    
  SPBRGH = 1;

  RCSTA = (NINE_BITS|0x90);	
  TXSTA = (SPEED|NINE_BITS|0x20);
}
