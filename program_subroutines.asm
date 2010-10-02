#define NUM_INSTRUCTIONS .14

;Runs the command corresponding to the value of "instruction".
;The return value is place at the top of the stack (i.e. stack)
;Requires piclang.asm
RUN_COMMAND_MAC macro instruction,programCounter
RUN_COMMAND movlw NUM_INSTRUCTIONS
	subwf instruction,W
	btfsc STATUS,DC
	goto $+3
	incf instruction,F
	return
	movlw HIGH RUN_COMMAND_TABLE
	movwf PCLATH
	movf instruction,W
	addwf PCL,F
RUN_COMMAND_TABLE	goto lda		;0x0
	goto adda		;0x1
	goto suba
	goto movaf		
	goto pusha		
	goto anda		
	goto ora		
	goto xora
	goto rra
	goto rla
	goto sleepa
	goto inca
	goto deca
	goto bsa
	goto bca
	goto clra
	goto sett	
	goto setd
	goto seta
END_OF_FUNCTION incf programCounter,F
	return
	
;Generic set subroutine. Can set current time, alarm time or date.
;Input: Stack values from top to bottom: most significant value, 
;			least significant value.
;Output: null
sett call GET_ARG
	call GET_ARG
	movlw TIME_ADDR
SET_GENERIC_TIMEDATE movwf FSR
	call POP_STACK
	movwf INDF
	incf FSR,F
	call POP_STACK
	movwf INDF
	goto END_OF_FUNCTION
;setd uses SET_GENERIC_TIMEDATE
setd call GET_ARG
	call GET_ARG
	movlw DATE_ADDR
	goto SET_GENERIC_TIMEDATE
seta call GET_ARG
	call GET_ARG
	movlw ALARM_ADDR
	goto SET_GENERIC_TIMEDATE
	;
lda call GET_ARG
	call POP_STACK
	movwf accumulator
	goto END_OF_FUNCTION
adda call GET_ARG
	call POP_STACK
	addwf accumulator,F
	goto END_OF_FUNCTION
suba call GET_ARG
	call POP_STACK
	subwf accumulator,F
	goto END_OF_FUNCTION
SWAP_STACK_X call GET_ARG
	call POP_STACK
	movwf exchange
	goto END_OF_FUNCTION
movaf	movf accumulator,W
	call PUSH_STACK
	call GET_ARG;places F on the stack
	call WRITE_EEPROM
	goto END_OF_FUNCTION
anda call GET_ARG
	call POP_STACK
	andwf accumulator,F
	goto END_OF_FUNCTION
ora call GET_ARG
	call POP_STACK
	iorwf accumulator,F
	goto END_OF_FUNCTION
xora call GET_ARG
	call POP_STACK
	xorwf accumulator,F
	goto END_OF_FUNCTION
pusha movf accumulator,W
	call PUSH_STACK
	goto END_OF_FUNCTION
sleepa call GET_ARG
	call GET_ARG
	call POP_STACK
	movwf exchange		;hour
	call POP_STACK
	movwf accumulator	;minute
	movf exchange,W
	subwf hours,W
	btfss STATUS,Z
	goto $-3
	movf accumulator,W
	subwf minutes,W
	btfss STATUS,Z
	goto $-3
	goto END_OF_FUNCTION
;Shifts accumulator to the right. Any amount carried over
;goes into exchange, as if exchange is to the right of
;accumulator
; 
;INPUT: The Amount of times shift is performed is on top
;	of the stack
rra	call GET_ARG
	call POP_STACK
	movwf instruction
	movlw 0xff
	andwf instruction,F
	btfsc STATUS,Z
	return			;in case we are told to rotate zero times :-/
	clrf exchange
	rrf accumulator,F
	btfss STATUS,C
	goto $+3
	rrf exchange,F
	bsf exchange,7
	decfsz instruction,F
	goto $-6
	goto END_OF_FUNCTION
	;
;Shifts accumulator to the left. Any amount carried over
;goes into exchange, as if exchange is to the left of
;accumulator
; 
;INPUT: The Amount of times shift is performed is on top
;	of the stack
rla	call GET_ARG
	call POP_STACK
	movwf instruction
	movlw 0xff
	andwf instruction,F
	btfsc STATUS,Z
	return			;in case we are told to rotate zero times :-/
	clrf exchange
	rlf accumulator,F
	btfss STATUS,C
	goto $+3
	rlf exchange,F
	bsf exchange,0
	decfsz instruction,F
	goto $-6
	goto END_OF_FUNCTION
	;
inca incf accumulator,F
	goto END_OF_FUNCTION
	;
deca decf accumulator,F
	goto END_OF_FUNCTION
	;
bsa call bsa_bca_setup
	iorwf accumulator,F
	goto END_OF_FUNCTION
	;
bca call bsa_bca_setup
	movwf tmp
	movlw 0xff
	xorwf tmp,W
	andwf accumulator,F
	goto END_OF_FUNCTION
	;
bsa_bca_setup call GET_ARG
	call POP_STACK
	movwf tmp
	incf tmp,F;1-indexed makes decfsz work
	clrf exchange
	bsf exchange,0
	decfsz tmp,F	
	rlf exchange,F
	movf exchange,W
	return
	;
clra clrf accumulator
	goto END_OF_FUNCTION
	
	endm;RUN_COMMAND_MAC