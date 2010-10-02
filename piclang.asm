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
READ_EEPROM macro stackPtr
	bcf STATUS,RP0;bank 0
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
	endm
	
;Writes data to EEPROM
;Uses FSR
;Input: EEADR is on the top of the stack and EEDATA is below it.
;Output: EEDATA is on the top of the stack.
;bank 0 is used after return
WRITE_EEPROM macro stackPtr 
	bcf STATUS,RP0;bank 0
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
	endm

PROGRAM_LOOP macro dipControl,INTPUT_BIT,instruction,RUN_PROGRAM,accumulator,exchange,PROGRAM_MAIN_FORK
	btfss dipControl,INPUT_BIT
	goto $-1
	movf dipControl,W
	movwf instruction
	bcf instruction,INPUT_BIT
	movlw 0x0		;0x0 = run program
	addwf instruction,W
	btfsc STATUS,Z
	goto RUN_PROGRAM
	btfss instruction,0	;xxxxxx1 = write A to x in EEPROM 
	goto $+9		;x will also be placed at the top of the stack.
	rrf instruction,F
	movf instruction,W
	call PUSH_STACK;this call is to keep the address, x, on the stack after this routine is finished.
	movf accumulator,W
	call PUSH_STACK
	movf instruction,W
	call PUSH_STACK
	call WRITE_EEPROM 	;end of write A to EEPORM
	goto PROGRAM_LOOP
	btfss instruction,2	;xxxx1?0  = place x on upper or lower nibble of A
	goto $+17
	btfsc instruction,1
	movlw 0xf0
	movlw 0xf
	swapf accumulator,F
	andwf accumulator,F
	swapf accumulator,F
	andwf instruction,W
	movwf exchange
	btfsc instruction,1
	goto $+4
	rrf exchange,F
	rrf exchange,F
	rrf exchange,F
	movf exchange,W
	addwf accumulator,F
	goto PROGRAM_MAIN_FORK
	call WRITE_EOP;xxxx010 = end of program
	endm

RUN_PROGRAM_MAC STACK_HEAD_ADDR,stackPtr,programCounter,READ_EEPROM,instruction,LEOP,REOP,instruction,RUN_COMMAND
RUN_PROGRAM movlw STACK_HEAD_ADDR
	movwf stackPtr
	movlw 0x0
	movwf programCounter
RUN_PROGRAM_LOOP movf programCounter,W
	call READ_EEPROM
	movwf instruction
	movlw LEOP
	subwf instruction,W
	btfsc STATUS,Z
	goto $+3
	call RUN_COMMAND;requires piclang.asm
	goto RUN_PROGRAM_LOOP
	movlw REOP
	subwf instruction,W
	btfsc STATUS,Z
	goto $+3
	call RUN_COMMAND
	goto RUN_PROGRAM_LOOP
	return
	endm
	
GET_ARG_MAC macro programCounter,READ_EEPROM,PUSH_STACK
GET_ARG incf programCounter,F
	movf programCounter,W
	call READ_EEPROM
	goto PUSH_STACK
	endm
	