	;
	;interface software 0.1
	;Author: David Coss
	;Copyright: Free to use with attribution to the Author. 
	;
	; See bottom for description of changes per version.
	
include <p16F690.inc>
     __config (_INTRC_OSC_NOCLKOUT & _WDT_OFF & _PWRTE_OFF & _MCLRE_OFF & _CP_OFF & _BOR_OFF & _IESO_OFF & _FCMEN_OFF)
	
#define COM PORTA
#define TRIS_COM TRISA
#define INST PORTC
#define TRIS_INST TRISC
#define E 2
#define nRW 1
#define RS 0
#define CONTRAST 0
#define HALF_SEC_PERIOD 0x2
#define LCD_LINE_SIZE 0x8

LCD_CHAR_MODE macro
	bsf COM,RS
	endm

LCD_INST_MODE macro
	bcf COM,RS
	endm

cblock     0x20
Instruction
SecCounter
FourtyMSCounter
DelayLoopCounter
WriteBuffer:LCD_LINE_SIZE * 2
curr_char
buffer_counter
     endc

org 0x0
goto INIT

INIT call HALF_SEC_DELAY 
	LCD_INST_MODE
	call HALF_SEC_DELAY
	call LCD_INIT
	call HALF_SEC_DELAY
	LCD_CHAR_MODE
	call HALF_SEC_DELAY
	movlw low WriteBuffer
	movwf FSR
	movlw 0x0
	movwf curr_char
	movlw LCD_LINE_SIZE * 2
	movwf buffer_counter
FILL_BUFFER_LOOP movf curr_char,W
	incf curr_char,F
	call MESSAGE
	movwf INDF
	incf FSR,F
	decfsz buffer_counter,F
	goto FILL_BUFFER_LOOP
	movlw LCD_LINE_SIZE
	movwf buffer_counter
	movlw low WriteBuffer
	movwf FSR
	call WRITE_LINE
	LCD_INST_MODE
	call HALF_SEC_DELAY
	movlw 0xC0
	call WRITE_CHAR
	call HALF_SEC_DELAY
	movlw low WriteBuffer
	movwf FSR
	movlw LCD_LINE_SIZE
	movwf buffer_counter
	call WRITE_LINE
LOOP nop
	goto $-1

WRITE_CHAR movwf Instruction
	swapf Instruction,F
	movlw 0xf
	andwf Instruction,W
	movwf INST
	call PULSE
	swapf Instruction,F
	movlw 0xf
	andwf Instruction,W
	movwf INST
	call PULSE
	return
WAKE movlw 0x3
	movwf INST
PULSE bsf COM,E
	nop
	nop
	nop
	nop
	bcf COM,E
	return
SEC_DELAY call HALF_SEC_DELAY
	call HALF_SEC_DELAY
	return
HALF_SEC_DELAY movlw HALF_SEC_PERIOD
	movwf SecCounter
	call FOURTY_MS_DELAY
	decfsz SecCounter,F
	goto $-2
	return
FOURTY_MS_DELAY movlw 0xff
	movwf FourtyMSCounter
	call DELAY
	decfsz FourtyMSCounter,F
	goto $-2
	return
DELAY movlw 0xff
	movwf DelayLoopCounter
DELAYLOOP decfsz DelayLoopCounter,F
	goto DELAYLOOP
	return
LCD_INIT bcf STATUS,RP0
	bcf STATUS,RP1 ;bank 0
	clrf PORTA 
	clrf PORTB
	clrf PORTC 
	bsf  STATUS,RP1 ; bank 2
	clrf ANSEL
	bsf STATUS,RP0
	bcf STATUS,RP1 ; bank 1
	movlw 0x0
	movwf TRISC
	movlw b'001000'
	movwf TRISA
	bcf STATUS,RP0 ; bank 0
	call HALF_SEC_DELAY
	call HALF_SEC_DELAY
	call HALF_SEC_DELAY
	call HALF_SEC_DELAY
	movlw 0x3
	movwf INST
	call PULSE
	call HALF_SEC_DELAY
	movlw 0x3
	movwf INST
	call PULSE
	call HALF_SEC_DELAY
	movlw 0x3
	movwf INST
	call PULSE
	call HALF_SEC_DELAY
	movlw 0x2
	movwf INST
	call PULSE
	movlw 0x28
	call WRITE_CHAR
	movlw 0x10
	call WRITE_CHAR
	movlw 0x0f
	call WRITE_CHAR
	movlw 0x06
	call WRITE_CHAR
	movlw 0x01
	call WRITE_CHAR
	movlw 0x02
	goto WRITE_CHAR
blah movlw 0x2
	movwf INST
	call PULSE
	call HALF_SEC_DELAY
	movlw 0x8
	movwf INST
	call PULSE
	call HALF_SEC_DELAY
	movlw 0x1
	movwf INST
	call PULSE
	call HALF_SEC_DELAY
	movlw 0x0
	movwf INST
	call PULSE
	call HALF_SEC_DELAY
	movlw 0x0
	movwf INST
	call PULSE
	call HALF_SEC_DELAY
	movlw 0xf
	movwf INST
	call PULSE
	call HALF_SEC_DELAY
	movlw 0x0
	movwf INST
	call PULSE
	call HALF_SEC_DELAY
	movlw 0x6
	movwf INST
	call PULSE
	call HALF_SEC_DELAY
	movlw 0x0
	movwf INST
	call PULSE
	call HALF_SEC_DELAY
	movlw 0x2
	movwf INST
	call PULSE
	call HALF_SEC_DELAY
	return
WRITE_LINE movf INDF,W
	call WRITE_CHAR
	call HALF_SEC_DELAY
	incf FSR,F
	decfsz buffer_counter,F
	goto WRITE_LINE
	return
MESSAGE movwf Instruction
	movlw HIGH MESSAGE_TABLE
	movwf PCLATH
	movf Instruction,W
	addwf PCL,F
MESSAGE_TABLE dt "David was here! "
CLEAR_MESSAGE dt "                " 
	END