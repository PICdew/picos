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
usart_getch() {
	/* retrieve one byte */
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

