#ifndef PICOS_DEFINES_H
#define PICOS_DEFINES_H 1


#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 1
#endif

// button stuff
#define button_bits PORTAbits
#define button_port PORTA
#define button_phase 3 // The amount by which the port bits should be right shifted to put the 1st button in the 0-th bit place

// LCD STUFF
#define	LCD_RS RA0
#define	LCD_RW RA1
#define LCD_EN RA2
#define LCD_DATA	PORTC



#endif //PICOS_DEFINES_H
