#include "arg.h"
#include "string.h"
#include "picfs_error.h"

#ifndef NO_PICLANG
#include "piclang.h"
#endif

char ARG_buffer[ARG_SIZE];


void ARG_clear()
{
  char i = 0;
  for(;i<ARG_SIZE;i++)
    ARG_buffer[i] = 0;
  ARG_next = ARG_SIZE;
  ARG_end = 0;
}

void ARG_putch(char ch)
{
  if(ARG_next >= ARG_SIZE)
    ARG_next = 0;
  ARG_buffer[ARG_end++] = ch;
}

char ARG_next_int()
{
  char retval = -1;
#ifndef NO_PICLANG
  if(ARG_source == ARG_PICLANG)
	return PICLANG_pop();
#endif

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

const char* ARG_gets()
{
  const char *next_word;
  if(ARG_next >= ARG_SIZE)
    return "";
  next_word = &ARG_buffer[ARG_next];
  ARG_next += 1 + strlen(next_word);
  return next_word;
}

