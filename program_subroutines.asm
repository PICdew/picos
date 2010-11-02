#define NUM_INSTRUCTIONS .14
;Retvals
#define ERROR_INVALID_INSTRUCTION 0x1

RUN_COMMAND_TABLE nop
	PICLANG_COMMAND_TABLE_MAC
	goto SETT	
	goto setd
	goto seta
	goto sleepa
	goto SHOW_CLOCK



SETT call GET_ARG
	call GET_ARG
	movlw low minutes
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
	movlw low dateDay
	goto SET_GENERIC_TIMEDATE
seta call GET_ARG
	call GET_ARG
	movlw alarmMinutes
	goto SET_GENERIC_TIMEDATE
	;
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
