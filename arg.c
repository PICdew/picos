/**
 * PICOS, PIC operating system.
 * Author: David Coss, PhD
 * Date: 1 Sept 2011
 * License: GNU Public License version 3.0 (see http://www.gnu.org)
 *
 * This file has the code for command argument functions.
 */
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
  if(ch == 0x7f)
    {
      if(ARG_end >= ARG_SIZE || ARG_end == 0)
	return;
      ARG_end--;
      ARG_buffer[ARG_end] = 0;
      return;
    }
  if(ARG_next >= ARG_SIZE)
    ARG_next = 0;
  ARG_buffer[ARG_end++] = ch;
}

signed char ARG_getd(picos_size_t *retval)
{
  if(retval == NULL)
    return error_return(PICFS_EINVAL);
  
  if(ARG_next >= ARG_SIZE)
    return error_return(ARG_BUFFER_OVERFLOW);

  *retval = 0;
  while(ARG_buffer[ARG_next] != 0 && ARG_buffer[ARG_next] != ' ')
    {
      *retval *= 10;
      if(ARG_buffer[ARG_next] > 0x39 || ARG_buffer[ARG_next] < 0x30)
	return error_return(ARG_INVALID);

      *retval += ARG_buffer[ARG_next] - 0x30;
      ARG_next++;
      if(ARG_next == ARG_SIZE)
	break;
    }
  ARG_next++;
  return SUCCESS;
}

signed char ARG_getch()
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
    return NULL;
  next_word = &ARG_buffer[ARG_next];
  if(*next_word == 0)
    return NULL;// end of args
  while(ARG_next != ARG_SIZE)
    {
      if(ARG_buffer[ARG_next] == ' ')
	{
	  ARG_buffer[ARG_next] = 0;
	  ARG_next++;
	  break;
	}
      ARG_next++;
    }
  return next_word;
}

char ARG_count()
{
  char count = 1;
  while(ARG_gets() != NULL)
    count++;
  return count;
}

signed char ARG_get(char index)
{
  char count = 0;
  const char *curr = ARG_buffer;

  for(;count < ARG_SIZE;count++)
    if(ARG_buffer[count] == 0)
      ARG_buffer[count] = ' ';
       
  count = 0;
  ARG_next = 0;
  curr = ARG_gets();
  while(curr != NULL)
    {
      if(count == index)
	break;
      curr = ARG_gets();
      count++;
    }

  if(curr == NULL)
    return -1;
  return (signed char)(curr - ARG_buffer);
}


