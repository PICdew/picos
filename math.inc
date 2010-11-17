ARITHMETIC_MAC macro math_temp,PUSH_STACK,POP_STACK,HEX_TO_CHAR
INT_TO_STR call POP_STACK
	banksel math_temp
	movwf math_temp
	xorlw 0xf
	call HEX_TO_CHAR
	call PUSH_STACK
	swapf math_temp,W
	xorlw 0xf
	call HEX_TO_CHAR
	call PUSH_STACK
	movlw ' '
	btfsc math_temp,7
	movlw '-'
	call PUSH_STACK
	bcf STATUS,RP0
	bcf STATUS,RP1
	return

FLOAT_TO_STR
	