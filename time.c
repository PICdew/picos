
/**
 * Simple Real time clock example by Garth Klee.
 * If run on PICDEM2+, time will be displayed
 * on LCD and will incorporate LED pendulum.
 * Has been written to run on PIC16F87x/A
 */

#include <htc.h>
#include <stdio.h>
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


#if 0
void main(void){
	init();
#if SCREEN == ON
	lcd_init();
#endif

	newSecond = tickCounter = 0;

	// Initialise the current time
	hours = START_H;
	minutes = START_M;
	seconds = START_S;
	ampm = START_AP;

	// Measure time
	while(1){
		if(newSecond){
			// A second has accumulated, count it
			newSecond--;
			if(++seconds > 59){
				seconds=0;
				if(++minutes > 59){
					minutes = 0;
					hours++;
					if(hours == 12)
						ampm^=1;
					if(hours>12)
						hours=1;
				}
			}

#if SCREEN == ON
			// print time on LCD screen
			lcd_goto(0);
			lcd_puts("Time=");
			// Print hours
			if(hours/10)
				lcd_putch('1');
			else
				lcd_putch(' ');
			lcd_putch((hours%10)+'0');
			lcd_putch(':');
			// print minutes
			lcd_putch((minutes/10)+'0');
			lcd_putch((minutes%10)+'0');
			lcd_putch(':');
			// print seconds
			lcd_putch((seconds/10)+'0');
			lcd_putch((seconds%10)+'0');
			if(ampm)
				lcd_putch('P');
			else
				lcd_putch('A');
			lcd_putch('M');
#endif

#if PENDULUM == ON
			// Rotate LED pattern every second
			PORTB=(pattern[seconds%sizeof(pattern)]);
#endif
#if TICKING == ON
			RC2^=1;	// tick effect
#endif

		}
#if ALARM == ON
		// If time matches alarm setting, toggle RC2
		if((hours == ALARM_H) &&
				(minutes == ALARM_M) &&
				(ampm == ALARM_AP)&&
				(seconds < ALARM_LENGTH)){
			unsigned int tone;
			RC2 ^= 1;	// generate buzz
			tone=TONE2;
			if(seconds&1)
				tone=TONE1;
			while(tone--)continue;	// tone generation
			RC2 ^= 1;	// generate buzz
		}
#endif
	}
}
#endif

