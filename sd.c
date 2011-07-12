#include "sd.h"
#include "error.h"

#include <htc.h>
#include <stddef.h>
#include <stdio.h>

char SPI_sendrecv(char data)
{
  SSPBUF = data;
  while (!BF);//blocking part
  return SSPBUF;
}

/**
 * Note: addr is Big endian, i.e. addr[0] is the MSB of the address
 *       This should be a four-byte array.
 */
char SPI_command(char frame1, char *addr, char frame2 )
{	
  char i, res;
  SPI_sendrecv(0xFF);
  SPI_sendrecv((frame1 | 0x40) & 0x7F);	// first 2 bits are 01. Remaining are command
  for(i = 0;i<4;i++)
    {
      if(addr != NULL)
	SPI_sendrecv(addr[i]);
      else
	SPI_sendrecv(0);
    }
  SPI_sendrecv(frame2|1);// CRC (which may be ignored by card) and last bit is always 1

  for(i=0;i<10;i++)	// wait for received character
    {
      res = SPI_sendrecv(0xFF);// Keep Card input high while waiting for response.
      if(res != 0xFF)
	break;
    }
  return res;	  
}

void SD_restart(char block_size)
{
  char i, block_size_arr[4];
	
  __delay_ms(2);

  for(i=0; i < 10; i++)
    SPI_sendrecv(0xFF);// dummy startup clock pulse (need at least 74)

  // Power up by setting CS low, and then sending CMD0 (reset)
  SD_CS=0;	
  if(SPI_command(0x00,NULL,0x95) !=1)
    {
      error_code = SD_INIT_ERROR;
      return;
    }

  while(SPI_command(0x01,NULL,0xFF) != 0);// Wait response idle state to end (first bit is idle flag)

  block_size_arr[0] = block_size_arr[1] = block_size_arr[2] = 0;
  block_size_arr[3] = 1;
  if(SPI_command(16,block_size_arr,0xFF) != 0)
    {
      error_code = SD_BLOCK_ERROR;
      return;
    }
	
  SD_CS = 1;
}

void SD_write(char *buff, char *loc)	
{
  unsigned int resp,i;
  SD_CS = 0;	
	
  resp = SPI_command(25,loc,0xFF);// multi block write (write until null char in buff)
  if(resp != 0)
    {
      error_code = SD_WRITE_ERROR;
      return;
    }
  SPI_sendrecv(0xFF);
  SPI_sendrecv(0xFF);
  SPI_sendrecv(0xFF);
	

  SPI_sendrecv(0xFC);//multi block write (CMD25) token
  while(*buff != 0)
    {
      SPI_sendrecv(*buff);
      buff++;
    }
  SPI_sendrecv(0xFF);
  SPI_sendrecv(0xFF);
	
  if((resp=SPI_sendrecv(0xFF) & 0x0F) == 0x05)//data accepted		
    {	
      for(i=10000;i>0;i--){				
	if(resp=SPI_sendrecv(0xFF))
	  break;
      }	
    }
  else
    {
      error_code = SD_WRITE_ERROR;
      return;
    }
  while(SPI_sendrecv(0xFF) != 0xFF);// wait until not busy

  SPI_sendrecv(0xFD);	//stop transfer	token byte
			
  SPI_sendrecv(0xFF);
  SPI_sendrecv(0xFF);
  while(SPI_sendrecv(0xFF) != 0xFF);// wait untile not busy
	
  SD_CS = 1;
}

void SD_read(char *addr, char *buffer, char n)
{
  int i,resp;
  SD_CS = 0;
  resp = SPI_command(18,addr,0xFF);	//read multi byte
  if(resp != 0)
    {
      error_code = SD_READ_ERROR;
      return;
    }

  while(SPI_sendrecv(0xFF) != 0xFE);	// wait for first byte
  
  for(i=0;i<n;i++)
    {
      *buffer = SPI_sendrecv(0xFF);
      buffer++;
    }
  SPI_sendrecv(0xFF);	//discard of CRC
  SPI_sendrecv(0xFF);
	
  SPI_command(12,NULL,0xFF);	//stop transmit
  SPI_sendrecv(0xFF);
  SPI_sendrecv(0xFF);
  SD_CS = 1;
}

