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

#include <stddef.h>
#include <stdio.h>

#include "picos/io.h"
#include "picos/utils.h"
#include "picos/picfs_error.h"
#include "picos/picos_time.h"

static TIME_t TIME_curr;

void TIME_inc_minute()
{
  char maxdays;

  TIME_curr.minutes++;
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
      TIME_curr.seconds = 0;
      TIME_inc_minute();
    }
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

void TIME_sleep(char x)
{
	x += TIME_tickCounter;
	x = x % TIME_PERIOD;
	
	while(x != TIME_tickCounter);
}	

