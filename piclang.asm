;piclang
;Author David Coss
;Version 0.1 (Written 2010)
;License: GNU General Public License 3.0 (see http://www.gnu.org)
;
;This file is part of a package that provides EEPROM IO code, 
;Program management and assembly language to the 
;16F870 PIC microcontroller.
;
;Requires a stack, which stackPtr storing the address of the current
;	location of the stack.
;Requires instruction register.
;Requires exchange register.
;Requires definition of LEOP, REOP, which are the Left (first) and 
;	Right(last) bytes of the end of program (EOP) signal

;Reads data from EEPROM.
;Uses FSR
;Input: Top of stack will contain address from which to read eeprom data
;Output: Data from eeprom will be at the top of the stack.
;bank 0 is used after return
READ_EEPROM bcf STATUS,RP0;bank 0
	bcf STATUS,RP1
	movf stackPtr,W
	movwf FSR
	movf INDF,W
	bsf STATUS,RP1;bank 2
	bcf STATUS,RP0
	movwf EEADR
	bsf STATUS,RP0;bank 3
	bcf EECON1,EEPGD
	bsf EECON1,RD
	bcf STATUS,RP0;bank 2
	movf EEDATA,W
	bcf STATUS,RP1;bank 0
	movwf INDF
	return
	
;Writes data to EEPROM
;Uses FSR
;Input: EEADR is on the top of the stack and EEDATA is below it.
;Output: EEDATA is on the top of the stack.
;bank 0 is used after return
WRITE_EEPROM bcf STATUS,RP0;bank 0
	bcf STATUS,RP1
	movf stackPtr,W
	movwf FSR
	movf INDF,W
	bsf STATUS,RP1;bank 3
	bsf STATUS,RP0
	btfsc EECON1,WR;wait for the last write to finish
	goto $-1
	bcf STATUS,RP0;bank 0
	bcf STATUS,RP1
	movf stackPtr,W
	movwf FSR
	decf stackPtr,F
	movf INDF,W
	bsf STATUS,RP1;bank 2
	movwf EEADR
	bcf STATUS,RP1;bank 0
	movf stackPtr,W
	movwf FSR
	movf INDF,W
	bsf STATUS,RP1;bank 2
	movwf EEDATA
	bsf STATUS,RP0;bank 3
	bcf EECON1,EEPGD;use eeprom, not program flash
	bsf EECON1,WREN;enable write
	BCF INTCON,GIE;temporarily disable interrupts
	movlw 0x55
	movwf EECON2
	movlw 0xaa
	movwf EECON2
	bsf EECON1,WR;start writting
	bsf INTCON,GIE;re-enable interrupts
	bcf EECON1,WR
	bcf EECON1,WREN;disable write.
	bcf STATUS,RP0;bank 0
	bcf STATUS,RP1
	return
	
;Writes the end of program code(2-bytes), starting at address at the top
;of the stack.
WRITE_EOP call POP_STACK
	movwf instruction
	movlw LEOP
	call PUSH_STACK
	movf instruction,F
	call PUSH_STACK
	call WRITE_EEPROM
	movlw REOP
	call PUSH_STACK
	incf instruction,W
	call PUSH_STACK
	goto WRITE_EEPROM
;
POP_STACK movf stackPtr,W
	movwf FSR
	decf stackPtr,F
	movf INDF,W
	return
PUSH_STACK movwf exchange 
	incf stackPtr,F
	movf stackPtr,W
	movwf FSR
	movf exchange,W
	movwf INDF
	return;