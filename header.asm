	
	list p=16f870
	include <p16f870.inc>
	radix hex
	
#define outport PORTC
#define outportTRIS TRISC
#define controlPort PORTA
#define controlPortTRIS TRISA
#define dipControl PORTB
#define dipControlTRIS TRISB

#define LEOP 0xde;EOP bytes
#define REOP 0xad;see piclang.asm

;Retvals
#define ERROR_INVALID_INSTRUCTION 0x1

;In the follwing bit definitions, the first word/phrase indicates
;the meaning of Logic 1 and the second word/phrase indicates
;the meaning of Logic 0
;
;myStatus bits
#define HOUR_MIN_BIT 0
#define BINARY_7SEG_BIT 1
;N stands for nibble
#define LSN_MSG_7SEG_BIT 2
#define ALARM_ON_OFF_BIT 3
#define HEX_OCTAL_BIT 4
#define ALARM_FLAG_TOGGLE 0x8
	
;DIP input port bits
#define DISPLAY_DATE_BIT 0
#define DISPLAY_ALARM_BIT 1
#define INPUT_BIT 7

;control port bits
#define PROGRAM_MODE_BIT 0
#define ALARM_TRIGGER_BIT 1
#define TIMER0_CLK 4

