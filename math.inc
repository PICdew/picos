ARITHMETIC_MAC macro math_temp,PUSH_STACK,POP_STACK,HEX_TO_CHAR
;Integers are represented using 8-bit two's complement
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
	movlw '+'
	btfsc math_temp,7
	movlw '-'
	call PUSH_STACK
	bcf STATUS,RP0
	bcf STATUS,RP1
	return

;float is eight bit
;7-th bit is the sign (1 = negative)
;bits 6-4 are the, e
;Such that the power of two is e - 3
;the lower 4 bits are the mantissa, with an implied leading "1."(base 2)
;So the decimal representation is (-1)^<7>*10^(<6:4>-3)*1.<3:0> (base 2)
FLOAT_TO_STR call POP_STACK
	banksel math_temp
	swapf math_temp,W
	andlw 0xf ; mask out the mantissa. Then left with e and sign bit
	call PUSH_STACK ; e and sign are on the stack
	movlw 0xf
	andwf math_temp,F
	movlw 0x10
	addwf math_temp,F ; add the impled 1. Then math temp represents real mantissa
	call PEEK_STACK
	andlw 0x7 ; get rid of sign bit to see if this is a negative or positive power of two
	sublw 0x3
	btfsc STATUS,C
	goto NEGATIVE_EXPONENT_FTS
POSITIVE_EXPONENT_FTS call PEEK_STACK
	andlw 0x7 ; mask sign
	xorlw 0
	btfsc STATUS,Z
	goto HAVE_DECIMAL_FTS
	rlf math_temp,F
	call POP_STACK
	sublw 0x1
	call PUSH_STACK
	goto POSITIVE_EXPONENT_FTS
NEGATIVE_EXPONENT_FTS
HAVE_DECIMAL_FTS ;math_temp has decimal. push then push sign char
	endm	