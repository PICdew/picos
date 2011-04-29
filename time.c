#include <htc.h>
#include <stdio.h>
#include <string.h>
#include "utils.h"
#include "time.h"

static TIME_t TIME_curr;

void TIME_init(void){
	T0CS = 0;	// Clear to enable timer mode.
	PSA = 0;	// Clear to assign prescaler to Timer 0.

	PS2 = 1;	// Set up prescaler to 1:256.  
	PS1 = 1;
	PS0 = 1;

	INTCON = 0;	// Clear interrupt flag bits.
	T0IE = 1;	// Set Timer 0 to 0.  
	TMR0 = 0;

	TIME_curr.hours = 12;
	TIME_curr.minutes = 34;
	TIME_curr.seconds = 56;
	TIME_curr.month = 6;
	TIME_curr.day = 29;
	TIME_curr.year = 0;
	
	ei();	// Global interrupts enabled

}

void TIME_tick(void)
{
  char maxdays;

  TIME_curr.seconds++;
  TIME_tickCounter = 0;
  if(TIME_curr.seconds == 60)
    {
      TIME_curr.minutes++;
      TIME_curr.seconds = 0;
    }
  if(TIME_curr.minutes == 60)
    {
      TIME_curr.hours++;
      TIME_curr.minutes = 0;
    }
  if(TIME_curr.hours == 24)
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

const TIME_t* TIME_get()
{
  return &TIME_curr;
}

void TIME_set(TIME_t *t)
{
  if(t != NULL)
    TIME_curr = *t;
}


void strtime(char *str, const TIME_t *t)
{
  char pos = 0;
  if(str == NULL)
    return;
  if(t == NULL)
    t = &TIME_curr;
  
  hex_to_word(str,t->month);pos += 2;
  str[pos++] = '/';
  hex_to_word(&str[pos],t->day);pos += 2;
  str[pos++] = '\n';
  hex_to_word(&str[pos],t->hours);pos += 2;
  str[pos++] = ':';
  hex_to_word(&str[pos],t->minutes);pos += 2;
  str[pos] = 0;
}
