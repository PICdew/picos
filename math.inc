;Macros for 16-bit math (int and float)
ARITHMETIC_MAC macro math_temp,errorByte,PUSH_STACK,POP_STACK,SWAP_STACK,PEEK_STACK,HEX_TO_CHAR

;If X is on the stack with the MSB on top, then
;X is popped and -X is pushed onto the stack.
NEGATE_INT call POP_STACK ; 
	banksel math_temp
	movwf math_temp
	comf math_temp,W
	call SWAP_STACK
	movwf math_temp
	comf math_temp,W
	call SWAP_STACK
	movwf math_temp
	call POP_STACK
	andlw 1
	btfsc STATUS,C
	incf math_temp,F
	call PUSH_STACK
	movf math_temp,W
	bcf STATUS,RP0
	bcf STATUS,RP1
	goto PUSH_STACK

;Performs A-B, where A was placed into the stack first. Both
;number are assumed to be push into the stack with the MSB on
;top. The difference will be on the stack with the MSB on top.
SUB_INT call NEGATE_INT
	goto ADD_INT

;expents two number to be placed on the stack, one after the other with
; their least significant bytes going in first. Upon successful addition,
; the sum is place on the stack with the MSB on top.
; 
;ErrorByte: 0 upon success, INTEGER_OVERFLOW upon overflow
ADD_INT call POP_STACK
	banksel math_temp
	movwf math_temp ; MSB of first operand
	call POP_STACK
	call SWAP_STACK ; at this point LSB of first is on top of stack and MSB of second operand is in W-REG
	addwf math_temp,W
	btfsc STATUS,C
	goto ADD_INT_OVERFLOW2
	call SWAP_STACK
	movwf math_temp
	call POP_STACK
	call SWAP_STACK ; at this point lsb of first operand is in math_temp, lsb of second operand is on top of the stack and msb of sum is second from top of stack
	addwf math_temp,W
	btfsc STATUS,C
	goto AI_INC_MSB
	call SWAP_STACK
	call PUSH_STACK
	clrf errorByte
	goto END_OF_ADD_INT
AI_INC_MSB call SWAP_STACK
	addlw 1
	btfsc STATUS,C
	goto ADD_INT_OVERFLOW
	call PUSH_STACK
	clrf errorByte
	goto END_OF_ADD_INT
ADD_INT_OVERFLOW2 call POP_STACK
ADD_INT_OVERFLOW call POP_STACK
	movlw INTEGER_OVERFLOW
	movwf errorByte
END_OF_ADD_INT bcf STATUS,RP0
	bcf STATUS,RP1
	return

;Adds to floating point numbers. Expects them on the stack one 
;NOTE: assumes like signs. For unlike signs, the user should
;	make like signs and subtract. (this should be integrate
;	later)
;
;after the other with the sign/exponent byte on top.
ADD_FLOAT call POP_STACK
	banksel math_temp
	movwf math_temp
	call POP_STACK
	call SWAP_STACK ; now the sign/exponent byte of the first operand is on math_temp, the same byte of the second operand on top of the stack. Below that are the lsb's of the first and second operands respectively.
	call PEEK_STACK
	subwf math_temp,W
	btfsc STATUS,Z
	goto ADD_FLOAT_SUBROUTINE
	btfsc STATUS,C
	goto STACK_BIGGER_AF
	call POP_STACK ; if we're here, the first operand has the bigger exponent. So we need to adjust (and possible truncate) the second mantissa
	call POP_STACK
	call SWAP_STACK ; second mantissa is now on top of the stack
	movf math_temp,W
	call SWAP_STACK ; second mantissa is now in math_temp, exponent is on top of stack
	;
	;todo: 	make the adjust smaller exponent's mantissa. then add. 
	;NOTE: when rrf is done don't forget to add 0x80 to the first rrf result
	;to account for the fact that the value is normalized before rrf is done.
	;
STACK_BIGGER_AF
ADD_FLOAT_SUBROUTINE 
END_OF_ADD_FLOAT bcf STATUS,RP0
	bcf STATUS,RP1
	return 

;Integers are represented using 16-bit two's complement
;expects most significant byte on top of the stack.
;returns in bank 0
INT_TO_STR call POP_STACK
	banksel math_temp
	movwf math_temp
	btfss math_temp,7
	goto MAGNITUDE_SIGN_TO_STR
	comf math_temp,F
	call POP_STACK
	addlw 1
	btfsc STATUS,C
	incf math_temp,F
	call PUSH_STACK
	bsf math_temp,7
MAGNITUDE_SIGN_TO_STR call PEEK_STACK
	andlw 0xf
	call HEX_TO_CHAR
	call SWAP_STACK
	call PUSH_STACK
	movf stackPtr,W
	movwf FSR
	swapf INDF,W
	andlw 0xf
	call HEX_TO_CHAR
	call SWAP_STACK
	movlw 0xf
	andwf math_temp,W
	call HEX_TO_CHAR
	call PUSH_STACK
	swapf math_temp,W
	andlw 0x7
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
	return
	endm	