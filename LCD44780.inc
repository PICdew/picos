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
	return
	endm

LCD_INST_MODE macro
	bcf COM,RS
	return
	endm

;Code to fill a buffer using the MESSAGE lookup table. DOES NOT RETURN
FILL_BUFFER_MAC macro WriteBuffer,buffer_counter,curr_char,MESSAGE
	clrf curr_char
	movf curr_char,W
	incf curr_char,F
	call MESSAGE
	movwf INDF
	incf FSR,F
	decfsz buffer_counter,F
	goto $-6
	endm
	
WRITE_BUFFER_MAC macro WriteBuffer,buffer_counter,WRITE_LINE,LCD_CHAR_MODE,LCD_INST_MODE
WRITE_BUFFER call LCD_INST_MODE
	movlw 0x0 ; top line
	call WRITE_CHAR
	movlw LCD_LINE_SIZE
	movwf buffer_counter
	movlw low WriteBuffer
	movwf FSR
	call LCD_CHAR_MODE
	call WRITE_LINE
	call LCD_INST_MODE
	call HALF_SEC_DELAY
	movlw 0xC0 ; bottom line
	call WRITE_CHAR
	call HALF_SEC_DELAY
	movlw low WriteBuffer + LCD_LINE_SIZE
	movwf FSR
	movlw LCD_LINE_SIZE
	movwf buffer_counter
	call LCD_CHAR_MODE
	call WRITE_LINE
	return
	endm

;WRITE_LINE will write a whole line, but you have to set the FSR to the
; beginning of the buffer and buffer_counter to the amount of characters
; you want written. ALSO, insure that the LCD is on the correct line before
; calling WRITE_LINE
LCD_WRITE_MAC macro Instruction,instruction_port,command_port,HALF_SEC_DELAY
WRITE_CHAR movwf Instruction
	swapf Instruction,F
	movlw 0xf
	andwf Instruction,W
	movwf instruction_port
	call PULSE
	swapf Instruction,F
	movlw 0xf
	andwf Instruction,W
	movwf INST
	call PULSE
	return
WAKE movlw 0x3
	movwf INST
PULSE bsf command_port,E
	nop
	nop
	nop
	nop
	bcf command_port,E
	return
WRITE_LINE movf INDF,W
	call WRITE_CHAR
	call HALF_SEC_DELAY
	incf FSR,F
	decfsz buffer_counter,F
	goto WRITE_LINE
	return
	endm
	
	;provides a set of delay routines
	; requires two bytes to store loop counters
DELAY_MAC macro SecCounter,DelayLoopCounter
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
	endm

INIT_LCD_MAC macro HALF_SEC_DELAY,PULSE,WRITE_CHAR
LCD_INIT bcf STATUS,RP0
	bcf STATUS,RP1 ;bank 0
	clrf COM
	clrf INST
	bsf  STATUS,RP1 ; bank 2
	clrf ANSEL
	bsf STATUS,RP0
	bcf STATUS,RP1 ; bank 1
	movlw 0x0
	movwf TRIS_INST
	movlw b'001000'
	movwf TRIS_COM
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
	endm

	