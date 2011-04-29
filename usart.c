#include <htc.h>
#include <stdio.h>
#include "usart.h"

void 
usart_putch(unsigned char byte) 
{
	while(!TXIF) 
		continue;
	TXREG = byte;
}

unsigned char 
usart_getch() 
{
  if(USART_timeout == 0)
    USART_timeout = USART_DEFAULT_TIMEOUT;
	while(!RCIF)	/* set when register is not empty */
	  {
	    if(USART_timeout == 0)
	      return 0;
	    continue;
	  }
	USART_timeout = 0;
	return RCREG;	
}

unsigned char 
usart_getch_block() 
{
	while(!RCIF)	/* set when register is not empty */
	    continue;
	return RCREG;	
}

unsigned char
usart_getche(void)
{
	unsigned char c;
	putch(c = getch());
	return c;
}

void usart_init()
{
  RX_PIN = 1;	
  TX_PIN = 1;		  
  SPBRG = DIVIDER;     	
  RCSTA = (NINE_BITS|0x90);	
  TXSTA = (SPEED|NINE_BITS|0x20);
}
