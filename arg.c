#include "arg.h"
#include "error.h"
#include "piclang.h"

void ARG_clear()
{
  char i = 0;
  for(;i<ARG_SIZE;i++)
    ARG_buffer[i] = 0;
  ARG_next = ARG_SIZE;
}

void ARG_putch(char ch)
{
  if(ARG_next >= ARG_SIZE)
    ARG_next = 0;
  ARG_buffer[ARG_next++] = ch;
}

char ARG_next_int()
{
  char retval = -1;
  if(ARG_source == ARG_PICLANG)
	return PICLANG_pop();

  if(ARG_next >= ARG_SIZE)
    {
      error_code = ARG_BUFFER_OVERFLOW;
      return retval;
    }
  retval = 0;
  while(ARG_buffer[ARG_next] != 0 && ARG_buffer[ARG_next] != ' ')
    {
      retval *= 10;
      if(ARG_buffer[ARG_next] > 0x39 || ARG_buffer[ARG_next] < 0x30)
	{
	  error_code = ARG_INVALID;
	  return -1;
	}
      retval += ARG_buffer[ARG_next] - 0x30;
      ARG_next++;
      if(ARG_next == ARG_SIZE)
	break;
    }
  return retval;
}

char ARG_getch()
{
  if(ARG_next >= ARG_SIZE)
    {
      error_code = ARG_BUFFER_OVERFLOW;
      return -1;
    }
  return ARG_buffer[ARG_next++];
}
