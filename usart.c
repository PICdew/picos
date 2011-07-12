#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "usart.h"

#include <htc.h>
#include <stdio.h>
#include <stddef.h>



void usart_putch(unsigned char byte) 
{
  while(!TXIF) 
    continue;
  TXREG = byte;
}

void usart_puts(const char *str)
{
  if(str == NULL)
    return;
  while(*str != 0)
    usart_putch(*(str++));
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


void usart_9send(char addr)
{
  TXSTAbits.TX9 = 1;
  TXSTAbits.TX9D = 1;
  usart_putch(addr);
  TXSTAbits.TX9 = 0;
  TXSTAbits.TX9D = 0;
}

char usart_9recv(char addr)
{
  char recv;
  RCSTAbits.ADDEN = 1;
  RCSTAbits.RX9 = 1;
  while(TRUE)
    {
      recv = usart_getch();
      if(addr == USART_SLAVE || addr == recv)
	{
	  RCSTAbits.ADDEN = 0;
	  RCSTAbits.RX9 = 0;
	  if(addr == USART_SLAVE)
	    return recv;
	  addr = USART_SLAVE;
	}
    }
  return recv;
}

void usart_9ack(char addr)
{
      usart_9send(addr);
      usart_putch(USART_ACK);
}

void usart2eeprom(char addr)
{
  char last;
  
  while(TRUE)
    {
      last = usart_getch();
      if(last == 0xde)
	{
	  last = usart_getch();
	  if(last == 0xad)
	    return;
	  eeprom_write(addr++, 0xde);
	  eeprom_write(addr++, last);
	}
      else
	eeprom_write(addr++,last);
    }
}

void eeprom2usart(char addr, char size)
{
  size += addr;
  for(;addr < size;addr++)
    usart_putch(eeprom_read(addr));  
}

