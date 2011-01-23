;Macros for 16-bit math (int and float)
ARITHMETIC_MAC macro math_temp,math_temp2,errorByte,PUSH_STACK,POP_STACK,SWAP_STACK,PEEK_STACK,HEX_TO_CHAR

;Multiples a byte on top of the stack by 10
;Returns with the product in w-reg
MULT_BYTE_10 
	banksel math_temp
	call SWAP_STACK ; previous mathtemp on stack. Arg(A) in w-reg
	movwf math_temp
	call PUSH_STACK ; A in math_temp and top of stack
	rrf math_temp,F
	rrf math_temp,F
	rrf math_temp,W ; 8*A
	call SWAP_STACK
	movwf math_temp
	rrf math_temp,F ; 2*A
	call POP_STACK
	addlw math_temp ; 2*A + 8*A = 10*A
	call SWAP_STACK
	movwf math_temp ; restore previous mathtemp
	call BANK_0
	goto POP_STACK ; returns with 10*A in W-Reg

	endm

ARITHMETIC_MAC_TO_BE_ADDED macro math_temp,math_temp2,errorByte,PUSH_STACK,POP_STACK,SWAP_STACK,PEEK_STACK,HEX_TO_CHAR
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

;Adds to floating point numbers.
ADD_FLOAT call POP_STACK
	movwf math_temp
	call PEEK_STACK
	subwf math_temp,W
	btfsc STATUS,C
	goto ADD_FLOAT_EXP_BORROW
	movwf math_temp
	movlw 1
	movwf math_temp2
	goto ADD_FLOAT_DEAL_WITH_SIGN
ADD_FLOAT_EXP_BORROW movf math_temp,W
	call SWAP_STACK
	movwf math_temp
	call PEEK_STACK
	subwf math_temp,W
	movlw 2
	movwf math_temp2	
ADD_FLOAT_DEAL_WITH_SIGN call POP_STACK
	andlw 0x80 ; sign bit
	iorwf math_temp2,F ; math_temp2 now has sign bit and flag of which operand has the greater exponent
	call POP_STACK
	btfsc math_temp2,0 ; in case the second operand is greater
	call SWAP_STACK
	call PUSH_STACK
	movlw 0x7f
	andwf math_temp2,W
	bsf math_temp2,2 ; indicte this is the first mantissa shift, thus add 0x80 for the implied 1 before the decimal point
ADD_FLOAT_ADJUST_MANTISSA xorlw 0 ; counter starts in W-reg
	btfsc STATUS,Z
	goto ADD_FLOAT_NEGATE_MANTISSAE;finish with adjusted MANTISSA on top of the stack and other operand's mantissa below it
	sublw 1
	call SWAP_STACK
	call PUSH_STACK
	movf math_temp2,W
	call SWAP_STACK
	movwf math_temp2
	rrf math_temp2,W
	call SWAP_STACK	
	movwf math_temp2
	call POP_STACK
	btfss math_temp2,2
	addlw 0x80
	call SWAP_STACK ; counter in w-reg as it should be for the beginning of the loop
	bcf math_temp2,2
	goto ADD_FLOAT_ADJUST_MANTISSA		
ADD_FLOAT_NEGATE_MANTISSAE call POP_STACK
	btfsc math_temp2,7
	goto NEGATE_MANTISSA
	movwf math_temp2
	goto ADD_FLOAT_CHECK_OTHER_MANTISSA
ADD_FLOAT_NEGATE_MANTISSA movwf math_temp2
	compf math_temp2,F
	incf math_temp2,F
ADD_FLOAT_CHECK_OTHER_MANTISSA btfss math_temp,7
	goto ADD_FLOAT_DO_ADD
	movf math_temp,W
	call SWAP_STACK
	movwf math_temp
	compf math_temp,F
	incf math_temp,W
	call SWAP_STACK
	movwf math_temp
	bcf math_temp,7
ADD_FLOAT_DO_ADD call POP_STACK
	addwf math_temp2,F
	btfss STATUS,C
	goto ADD_FLOAT_CHECK_SUM_SIGN
	rrf math_temp2,F
	incf math_temp,W
	andlw 0x7f
	movwf math_temp
ADD_FLOAT_CHECK_SUM_SIGN btfss math_temp2,7
	goto ADD_FLOAT_END
	bsf math_temp,7
	compf math_temp2,F
	incf math_temp2,F
ADD_FLOAT_END movf math_temp2,W
	call PUSH_STACK
	movf math_temp,W
	goto PUSH_STACK; the END!!!

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