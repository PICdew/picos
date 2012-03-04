/**
 * PICOS, PIC operating system.
 * Author: David Coss, PhD
 * Date: 1 Sept 2011
 * License: GNU Public License version 3.0 (see http://www.gnu.org)
 *
 * This file provides time of day and date functions, with precision within seconds.
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "io.h"
#include "utils.h"
#include "picfs_error.h"
#include "picos_time.h"

#include <stddef.h>
#include <stdio.h>

static TIME_t TIME_curr;

void TIME_inc_minute()
{
  char maxdays;

  if(TIME_curr.minutes >= 60)
    {
      TIME_curr.hours++;
      TIME_curr.minutes = 0;
    }
  if(TIME_curr.hours >= 24)
    {
      TIME_curr.day++;
      TIME_curr.hours = 0;
    }
  
  switch(TIME_curr.month)
    {
    case 4:case 6:case 9:case 11:
      maxdays = 30;
      break;
    case 2:
      if(TIME_curr.year % 4 == 0)
	maxdays = 29;
      else
	maxdays = 28;
      break;
    default:
      maxdays = 30;
      break;
    }
  if(TIME_curr.day > maxdays)
    {
      TIME_curr.month++;
      TIME_curr.day = 1;//no zero days...
    }
  if(TIME_curr.month > 12)
    TIME_curr.month = 1;//no zero months...
}

void TIME_tick(void)
{
  TIME_curr.seconds++;
  TIME_tickCounter = 0;
  if(TIME_curr.seconds >= 60)
    {
      TIME_curr.minutes++;
      TIME_curr.seconds = 0;
    }
  TIME_inc_minute();
}

const TIME_t* TIME_get()
{
  return &TIME_curr;
}

void TIME_set(TIME_t *t)
{
  if(t != NULL)
    {
      if(t->hours > 23 || t->minutes > 59 || t->month > 12 || t->day > 31)
	{
	  error_code = TIME_INVALID;
	  return;
	}
      TIME_curr = *t;
    }
    
}


void strtime(char *str, const TIME_t *t)
{
  char pos = 0;
  if(str == NULL)
    return;
  if(t == NULL)
    t = TIME_get();
  
  hex_to_word(str,t->month);pos += 2;
  str[pos++] = '/';
  hex_to_word(&str[pos],t->day);pos += 2;
  str[pos++] = '\n';
  hex_to_word(&str[pos],t->hours);pos += 2;
  str[pos++] = ':';
  hex_to_word(&str[pos],t->minutes);pos += 2;
  str[pos] = 0;
}

void TIME_stdout()
{
  IO_putd(TIME_curr.month);
  putch('/');
  IO_putd(TIME_curr.day);
  putch('\n');
  IO_putd(TIME_curr.hours);
  putch(':');
  IO_putd(TIME_curr.minutes);
}

void TIME_sleep(char x)
{
	x += TIME_tickCounter;
	x = x % TIME_PERIOD;
	
	while(x != TIME_tickCounter);
}	

