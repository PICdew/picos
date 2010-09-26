;Runs the command corresponding to the value of "instruction".
;The return value is place at the top of the stack (i.e. stack)
;Requires piclang.asm
RUN_COMMAND movlw NUM_INSTRUCTIONS
	subwf instruction,W
	btfsc STATUS,DC
	goto $+3
	incf instruction,F
	return
	movlw HIGH RUN_COMMAND
	movwf PCLATH
	movf instruction,W
	addwf PCL,F
	goto LDA		;0x0
	goto ADD_A		;0x1
	goto SUBTRACT_A
	goto MOV_AF		
	goto PUSH_A		
	goto AND_A		
	goto OR_A		
	goto XOR_A
	goto RRA
	goto RLA
	goto SLEEP_UNTIL
	goto SET_TIME	
	goto SET_DATE
	goto SET_ALARM
END_OF_FUNCTION incf programCounter,F
	return
GET_ARG incf programCounter,F
	movf programCounter,W
	call READ_EEPROM
	goto PUSH_STACK
;Generic set subroutine. Can set current time, alarm time or date.
;Input: Stack values from top to bottom: most significant value, 
;			least significant value.
;Output: null
SET_TIME call GET_ARG
	call GET_ARG
	movlw TIME_ADDR
SET_GENERIC_TIMEDATE movwf FSR
	call POP_STACK
	movwf INDF
	incf FSR,F
	call POP_STACK
	movwf INDF
	goto END_OF_FUNCTION
;SET_DATE uses SET_GENERIC_TIMEDATE
SET_DATE call GET_ARG
	call GET_ARG
	movlw DATE_ADDR
	goto SET_GENERIC_TIMEDATE
SET_ALARM call GET_ARG
	call GET_ARG
	movlw ALARM_ADDR
	goto SET_GENERIC_TIMEDATE
	;
LDA call GET_ARG
	call POP_STACK
	movwf accumulator
	goto END_OF_FUNCTION
ADD_A call GET_ARG
	call POP_STACK
	addwf accumulator,F
	goto END_OF_FUNCTION
SUBTRACT_A call GET_ARG
	call POP_STACK
	subwf accumulator,F
	goto END_OF_FUNCTION
SWAP_STACK_X call GET_ARG
	call POP_STACK
	movwf exchange
	goto END_OF_FUNCTION
MOV_AF	movf accumulator,W
	call PUSH_STACK
	call GET_ARG;places F on the stack
	call WRITE_EEPROM
	goto END_OF_FUNCTION
AND_A call GET_ARG
	call POP_STACK
	andwf accumulator,F
	goto END_OF_FUNCTION
OR_A call GET_ARG
	call POP_STACK
	iorwf accumulator,F
	goto END_OF_FUNCTION
XOR_A call GET_ARG
	call POP_STACK
	xorwf accumulator,F
	goto END_OF_FUNCTION
PUSH_A movf accumulator,W
	call PUSH_STACK
	goto END_OF_FUNCTION
SLEEP_UNTIL call GET_ARG
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
RRA	call GET_ARG
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
RLA	call GET_ARG
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
